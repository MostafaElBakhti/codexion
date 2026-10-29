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

static int	start_compile(t_coder *coder, long now)
{
	coder->last_compile_start = now;
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	log_state(coder, "is compiling");
	pthread_cond_broadcast(&coder->sim->arbiter_cond);
	return (0);
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
			return (start_compile(coder, now));
		cooldown = next_cooldown(coder, now);
		if (cooldown && wait_until(sim, cooldown) != 0)
			return (1);
		if (!cooldown && pthread_cond_wait(&sim->arbiter_cond,
				&sim->arbiter_mutex) != 0)
			return (1);
	}
	return (2);
}

static int	acquire_single(t_coder *coder)
{
	t_simulation	*sim;
	int			result;

	sim = coder->sim;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	if (!sim->stop)
	{
		pthread_mutex_lock(&coder->left->mutex);
		coder->left->available = 0;
		pthread_mutex_unlock(&coder->left->mutex);
		log_state(coder, "has taken a dongle");
	}
	result = 0;
	while (!sim->stop && !result)
		result = pthread_cond_wait(&sim->arbiter_cond,
				&sim->arbiter_mutex);
	pthread_mutex_unlock(&sim->arbiter_mutex);
	if (result)
		return (1);
	return (2);
}

int	acquire_pair(t_coder *coder)
{
	t_simulation	*sim;
	int				result;

	if (coder->left == coder->right)
		return (acquire_single(coder));
	sim = coder->sim;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->arbiter_mutex);
		return (2);
	}
	result = submit_pair(coder);
	if (!result)
	{
		result = wait_for_pair(coder);
		if (result)
			cancel_pair(coder);
	}
	pthread_mutex_unlock(&sim->arbiter_mutex);
	return (result);
}
