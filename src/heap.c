/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_heap(t_heap *heap, int max_requests)
{
	if (!heap || max_requests <= 0)
		return (1);
	heap->requests = malloc(sizeof(t_request) * max_requests);
	if (!heap->requests)
		return (1);
	heap->request_count = 0;
	heap->max_requests = max_requests;
	return (0);
}

void	destroy_heap(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->requests);
	heap->requests = NULL;
	heap->request_count = 0;
	heap->max_requests = 0;
}

int	heap_push(t_heap *heap, t_request request, char *scheduler)
{
	int			i;
	int			parent;
	t_request	temp;

	if (heap->request_count >= heap->max_requests)
		return (1);
	i = heap->request_count;
	heap->requests[i] = request;
	heap->request_count++;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!higher_priority(heap->requests[i],
				heap->requests[parent], scheduler))
			break ;
		temp = heap->requests[i];
		heap->requests[i] = heap->requests[parent];
		heap->requests[parent] = temp;
		i = parent;
	}
	return (0);
}

int	heap_pop(t_heap *heap, t_request *request, char *scheduler)
{
	if (!heap || !request || heap->request_count == 0)
		return (1);
	*request = heap->requests[0];
	heap->request_count--;
	if (heap->request_count > 0)
	{
		heap->requests[0] = heap->requests[heap->request_count];
		heap_repair(heap, 0, scheduler);
	}
	return (0);
}
