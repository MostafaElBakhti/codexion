/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   acquire.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/28 00:30:47 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	next_cooldown(t_coder *coder, long now)
{
	long	next;

	next = coder->left->cooldown_until;
	if (coder->right->cooldown_until > next)
		next = coder->right->cooldown_until;
	if (next <= now)
		return (0);
	return (next);
}

static int	wait_for_pair(t_coder *coder)
{
	t_simulation	*sim;
	long			now;
	long			cooldown;

	sim = coder->sim;
	while (!sim->stop)
	{
		now = get_time_ms();
		if (now < 0)
			return (1);
		if (reserve_pair(coder, now))
		{
			coder->last_compile_start = now;
			return (0);
		}
		cooldown = next_cooldown(coder, now);
		if (cooldown && wait_until(sim, cooldown) != 0)
			return (1);
		if (!cooldown && pthread_cond_wait(&sim->arbiter_cond,
				&sim->arbiter_mutex) != 0)
			return (1);
	}
	return (1);
}

int	acquire_pair(t_coder *coder)
{
	t_request		request;
	t_simulation	*sim;
	int				result;

	if (coder->left == coder->right)
		return (2);
	sim = coder->sim;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	request.request_time = get_time_ms();
	request.coder_id = coder->id;
	request.deadline = coder->last_compile_start
		+ sim->config.time_to_burnout;
	result = (request.request_time < 0);
	if (!result)
		result = queue_pair(coder, request);
	if (!result)
	{
		result = wait_for_pair(coder);
		if (result)
			cancel_pair(coder);
	}
	pthread_mutex_unlock(&sim->arbiter_mutex);
	return (result);
}
