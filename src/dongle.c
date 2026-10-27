/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	lock_pair(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left;
	second = coder->right;
	if (first->id > second->id)
	{
		first = coder->right;
		second = coder->left;
	}
	pthread_mutex_lock(&first->mutex);
	pthread_mutex_lock(&second->mutex);
}

void	unlock_pair(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left->mutex);
	pthread_mutex_unlock(&coder->right->mutex);
}

int	release_pair(t_coder *coder)
{
	t_simulation	*sim;
	long			now;

	sim = coder->sim;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	lock_pair(coder);
	now = get_time_ms();
	coder->left->available = 1;
	coder->right->available = 1;
	coder->left->cooldown_until = now + sim->config.dongle_cooldown;
	coder->right->cooldown_until = now + sim->config.dongle_cooldown;
	unlock_pair(coder);
	pthread_cond_broadcast(&sim->arbiter_cond);
	pthread_mutex_unlock(&sim->arbiter_mutex);
	return (now < 0);
}

void	destroy_dongles(t_simulation *sim)
{
	int	i;

	if (!sim->dongles)
		return ;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		destroy_heap(&sim->dongles[i].heap);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
		i++;
	}
	free(sim->dongles);
	sim->dongles = NULL;
}
