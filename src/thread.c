/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_simulation *sim)
{
	pthread_mutex_lock(&sim->arbiter_mutex);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->arbiter_cond);
	pthread_mutex_unlock(&sim->arbiter_mutex);
}

void	log_state(t_coder *coder, char *state)
{
	t_simulation	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d %s\n", get_time_ms() - sim->start_time,
		coder->id, state);
	pthread_mutex_unlock(&sim->log_mutex);
}

static int	compile_once(t_coder *coder)
{
	t_simulation	*sim;
	long			end;
	int				result;

	sim = coder->sim;
	log_state(coder, "has taken a dongle");
	log_state(coder, "has taken a dongle");
	log_state(coder, "is compiling");
	end = get_time_ms() + sim->config.time_to_compile;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	result = 0;
	while (!sim->stop && get_time_ms() < end && !result)
		result = wait_until(sim, end);
	pthread_mutex_unlock(&sim->arbiter_mutex);
	if (release_pair(coder) != 0)
		result = 1;
	return (result);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	int		result;

	coder = (t_coder *)arg;
	result = acquire_pair(coder);
	if (result == 2)
		return (NULL);
	if (result == 0)
		result = compile_once(coder);
	if (result != 0)
	{
		stop_simulation(coder->sim);
		return ((void *)1);
	}
	return (NULL);
}
