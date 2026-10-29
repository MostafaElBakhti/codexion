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

static void	record_compile(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	/* Caller holds arbiter_mutex, including the completion stop decision. */
	coder->completed_compiles++;
	if (coder->completed_compiles == sim->config.number_of_compiles_required)
		sim->completed_coders++;
	if (sim->completed_coders == sim->config.number_of_coders)
	{
		sim->stop = 1;
		pthread_cond_broadcast(&sim->arbiter_cond);
	}
}

static int	compile_once(t_coder *coder)
{
	t_simulation	*sim;
	long			end;
	int				result;

	sim = coder->sim;
	end = get_time_ms() + sim->config.time_to_compile;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return (1);
	result = 0;
	while (!sim->stop && get_time_ms() < end && !result)
		result = wait_until(sim, end);
	if (sim->stop && !result)
		result = 2;
	if (!result)
		record_compile(coder);
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
	int		required;
	int		result;

	coder = (t_coder *)arg;
	required = coder->sim->config.number_of_compiles_required;
	result = 0;
	while (!result && coder->completed_compiles < required)
	{
		result = acquire_pair(coder);
		if (!result)
			result = compile_once(coder);
		if (!result && coder->completed_compiles < required)
			result = run_phase(coder, "is debugging",
					coder->sim->config.time_to_debug);
		if (!result && coder->completed_compiles < required)
			result = run_phase(coder, "is refactoring",
					coder->sim->config.time_to_refactor);
	}
	if (result == 1)
		stop_simulation(coder->sim);
	return ((void *)(long)(result == 1));
}
