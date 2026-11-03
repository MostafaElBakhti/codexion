/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/11/01 20:19:17 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	wait_initial_turn(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	if (coder->completed_compiles != 0)
		return (0);
	while (coder->id % 2 != 0
		&& sim->initial_even_requests < sim->config.number_of_coders / 2
		&& !sim->stop)
	{
		if (pthread_cond_wait(&sim->arbiter_cond,
				&sim->arbiter_mutex) != 0)
			return (1);
	}
	if (sim->stop)
		return (2);
	return (0);
}

static int	join_threads(t_simulation *sim, int count, pthread_t monitor)
{
	int		failed;
	int		i;
	void	*result;

	failed = (count != sim->config.number_of_coders);
	if (failed)
		stop_simulation(sim);
	i = 0;
	while (i < count)
	{
		result = NULL;
		if (pthread_join(sim->coders[i].thread, &result) != 0 || result)
			failed = 1;
		i++;
	}
	result = NULL;
	if (pthread_join(monitor, &result) != 0 || result)
		failed = 1;
	return (failed);
}

int	start_threads(t_simulation *sim)
{
	int			i;
	pthread_t	monitor;

	i = 0;
	while (i < sim->config.number_of_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		i++;
	}
	if (pthread_create(&monitor, NULL, monitor_routine, sim) != 0)
		return (1);
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			break ;
		i++;
	}
	return (join_threads(sim, i, monitor));
}
