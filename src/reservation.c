/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reservation.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	queue_pair(t_coder *coder, t_request request)
{
	t_simulation	*sim;
	int				result;

	sim = coder->sim;
	lock_pair(coder);
	result = heap_push(&coder->left->heap, request, sim->config.scheduler);
	if (result == 0)
	{
		result = heap_push(&coder->right->heap, request,
				sim->config.scheduler);
		if (result != 0)
			heap_remove(&coder->left->heap, coder->id,
				sim->config.scheduler);
	}
	unlock_pair(coder);
	return (result);
}

static int	pair_ready(t_coder *coder, long now)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->left;
	right = coder->right;
	return (left->available && right->available
		&& now >= left->cooldown_until
		&& now >= right->cooldown_until
		&& left->heap.request_count > 0
		&& right->heap.request_count > 0
		&& left->heap.requests[0].coder_id == coder->id
		&& right->heap.requests[0].coder_id == coder->id);
}

int	reserve_pair(t_coder *coder, long now)
{
	t_request	winner;
	int			ready;

	lock_pair(coder);
	ready = pair_ready(coder, now);
	if (ready)
	{
		heap_pop(&coder->left->heap, &winner,
			coder->sim->config.scheduler);
		heap_pop(&coder->right->heap, &winner,
			coder->sim->config.scheduler);
		coder->left->available = 0;
		coder->right->available = 0;
	}
	unlock_pair(coder);
	return (ready);
}

void	cancel_pair(t_coder *coder)
{
	char	*scheduler;

	scheduler = coder->sim->config.scheduler;
	lock_pair(coder);
	heap_remove(&coder->left->heap, coder->id, scheduler);
	heap_remove(&coder->right->heap, coder->id, scheduler);
	unlock_pair(coder);
	pthread_cond_broadcast(&coder->sim->arbiter_cond);
}
