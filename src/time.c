/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <time.h>
#include <errno.h>

long	get_time_ms(void)
{
	struct timespec	ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		return (-1);
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int	wait_until(t_simulation *sim, long deadline)
{
	struct timespec	timeout;
	long			remaining;
	int				result;

	remaining = deadline - get_time_ms();
	if (remaining <= 0)
		return (0);
	if (clock_gettime(CLOCK_REALTIME, &timeout) != 0)
		return (1);
	timeout.tv_sec += remaining / 1000;
	timeout.tv_nsec += (remaining % 1000) * 1000000L;
	if (timeout.tv_nsec >= 1000000000L)
	{
		timeout.tv_sec++;
		timeout.tv_nsec -= 1000000000L;
	}
	result = pthread_cond_timedwait(&sim->arbiter_cond,
			&sim->arbiter_mutex, &timeout);
	return (result != 0 && result != ETIMEDOUT);
}
