/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/30 23:59:01 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static t_coder	*next_deadline(t_simulation *sim, long *deadline)
{
	t_coder	*next;
	long	candidate;
	int		i;

	next = NULL;
	i = 0;
	while (i < sim->config.number_of_coders)
	{
		if (sim->coders[i].completed_compiles
			< sim->config.number_of_compiles_required)
		{
			candidate = sim->coders[i].last_compile_start
				+ sim->config.time_to_burnout;
			if (!next || candidate < *deadline)
			{
				*deadline = candidate;
				next = &sim->coders[i];
			}
		}
		i++;
	}
	return (next);
}

static void	log_burnout(t_coder *coder)
{
	t_simulation	*sim;

	sim = coder->sim;
	sim->stop = 1;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d burned out\n", get_time_ms() - sim->start_time,
		coder->id);
	pthread_mutex_unlock(&sim->log_mutex);
	pthread_cond_broadcast(&sim->arbiter_cond);
}

static int	monitor_step(t_simulation *sim)
{
	t_coder		*next;
	long		deadline;
	long		now;

	next = next_deadline(sim, &deadline);
	now = get_time_ms();
	if (!next || now < 0)
		return (-1);
	if (now >= deadline)
	{
		log_burnout(next);
		return (1);
	}
	if (wait_until(sim, deadline) != 0)
		return (-1);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simulation	*sim;
	int			status;

	sim = (t_simulation *)arg;
	if (pthread_mutex_lock(&sim->arbiter_mutex) != 0)
		return ((void *)1);
	status = 0;
	while (!sim->stop && status == 0)
		status = monitor_step(sim);
	if (status < 0)
	{
		sim->stop = 1;
		pthread_cond_broadcast(&sim->arbiter_cond);
	}
	pthread_mutex_unlock(&sim->arbiter_mutex);
	return ((void *)(long)(status < 0));
}
