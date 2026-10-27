/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 01:05:49 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_sync(t_simulation *sim)
{
	if (pthread_mutex_init(&sim->arbiter_mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&sim->arbiter_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->arbiter_mutex);
		return (1);
	}
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
	{
		pthread_cond_destroy(&sim->arbiter_cond);
		pthread_mutex_destroy(&sim->arbiter_mutex);
		return (1);
	}
	return (0);
}

static void	destroy_sync(t_simulation *sim)
{
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_cond_destroy(&sim->arbiter_cond);
	pthread_mutex_destroy(&sim->arbiter_mutex);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;
	int				result;

	memset(&sim, 0, sizeof(sim));
	if (argc != 9 || parse_arguments(argc, argv, &sim.config) != 0)
		return (1);
	if (init_sync(&sim) != 0)
		return (1);
	result = init_dongles(&sim);
	if (!result)
		result = init_coders(&sim);
	if (!result)
	{
		sim.start_time = get_time_ms();
		result = (sim.start_time < 0);
		if (!result)
			result = start_threads(&sim);
	}
	free(sim.coders);
	destroy_dongles(&sim);
	destroy_sync(&sim);
	return (result);
}
