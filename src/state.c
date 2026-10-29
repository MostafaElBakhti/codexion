/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/29 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/29 00:00:00 by mel-bakh         ###   ########.fr       */
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
	/* Caller holds arbiter_mutex: stop and output order share one lock order. */
	if (sim->stop)
		return ;
	pthread_mutex_lock(&sim->log_mutex);
	printf("%ld %d %s\n", get_time_ms() - sim->start_time,
		coder->id, state);
	pthread_mutex_unlock(&sim->log_mutex);
}
