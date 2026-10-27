/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coders(t_simulation *sim)
{
	int	i;
	int	count;

	count = sim->config.number_of_coders;
	sim->coders = malloc(sizeof(t_coder) * count);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < count)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].right = &sim->dongles[i];
		sim->coders[i].left = &sim->dongles[(i + count - 1) % count];
		sim->coders[i].sim = sim;
		i++;
	}
	return (0);
}

static int	init_one_dongle(t_dongle *dongle, int id, int count)
{
	dongle->id = id;
	dongle->available = 1;
	dongle->cooldown_until = 0;
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (1);
	if (init_heap(&dongle->heap, count) != 0)
	{
		pthread_mutex_destroy(&dongle->mutex);
		return (1);
	}
	return (0);
}

int	init_dongles(t_simulation *sim)
{
	int	i;
	int	count;

	count = sim->config.number_of_coders;
	sim->dongles = malloc(sizeof(t_dongle) * count);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < count && init_one_dongle(&sim->dongles[i], i + 1,
			count) == 0)
		i++;
	if (i == count)
		return (0);
	while (--i >= 0)
	{
		destroy_heap(&sim->dongles[i].heap);
		pthread_mutex_destroy(&sim->dongles[i].mutex);
	}
	free(sim->dongles);
	sim->dongles = NULL;
	return (1);
}
