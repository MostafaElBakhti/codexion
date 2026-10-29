/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_order.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	higher_priority(t_request a, t_request b, char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
	{
		if (a.request_time != b.request_time)
			return (a.request_time < b.request_time);
		return (a.sequence < b.sequence);
	}
	else
	{
		if (a.deadline != b.deadline)
			return (a.deadline < b.deadline);
		/* Equal deadlines favor alternating seats, allowing parallel pairs. */
		if (a.coder_id % 2 != b.coder_id % 2)
			return (a.coder_id % 2 == 0);
		if (a.request_time != b.request_time)
			return (a.request_time < b.request_time);
	}
	return (a.sequence < b.sequence);
}

static void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	heap_repair(t_heap *heap, int i, char *scheduler)
{
	int	child;

	while (i > 0 && higher_priority(heap->requests[i],
			heap->requests[(i - 1) / 2], scheduler))
	{
		heap_swap(&heap->requests[i], &heap->requests[(i - 1) / 2]);
		i = (i - 1) / 2;
	}
	while (2 * i + 1 < heap->request_count)
	{
		child = 2 * i + 1;
		if (child + 1 < heap->request_count
			&& higher_priority(heap->requests[child + 1],
				heap->requests[child], scheduler))
			child++;
		if (!higher_priority(heap->requests[child],
				heap->requests[i], scheduler))
			break ;
		heap_swap(&heap->requests[i], &heap->requests[child]);
		i = child;
	}
}

int	heap_remove(t_heap *heap, int coder_id, char *scheduler)
{
	int	i;

	i = 0;
	while (i < heap->request_count
		&& heap->requests[i].coder_id != coder_id)
		i++;
	if (i == heap->request_count)
		return (1);
	heap->request_count--;
	if (i < heap->request_count)
	{
		heap->requests[i] = heap->requests[heap->request_count];
		heap_repair(heap, i, scheduler);
	}
	return (0);
}
