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
	if (sim->stop && !result)
		result = 2;
	pthread_mutex_unlock(&sim->arbiter_mutex);
	if (release_pair(coder) != 0)
		result = 1;
	return (result);
}

static int	run_phase(t_coder *coder, char *state, int duration)
{
	t_simulation	*sim;
	long			end;
	int			result;

	sim = coder->sim;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->arbiter_mutex);
		return (2);
	}
	log_state(coder, state);
	end = get_time_ms() + duration;
	result = 0;
	while (!sim->stop && get_time_ms() < end && !result)
		result = wait_until(sim, end);
	if (sim->stop && !result)
		result = 2;
	pthread_mutex_unlock(&sim->arbiter_mutex);
	return (result);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*config;
	int		compiles;
	int		result;

	coder = (t_coder *)arg;
	config = &coder->sim->config;
	compiles = 0;
	result = 0;
	while (compiles < config->number_of_compiles_required && !result)
	{
		result = acquire_pair(coder);
		if (!result)
			result = compile_once(coder);
		if (!result && ++compiles < config->number_of_compiles_required)
			result = run_phase(coder, "is debugging", config->time_to_debug);
		if (!result && compiles < config->number_of_compiles_required)
			result = run_phase(coder, "is refactoring", config->time_to_refactor);
	}
	if (result == 1)
		stop_simulation(coder->sim);
	return ((void *)(long)(result != 0));
}
