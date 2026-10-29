/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
