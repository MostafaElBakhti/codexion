#include "codexion.h"

int init_heap(t_heap *heap, int max_requests)
{
    if(!heap || max_requests <= 0)
        return (1);
    heap->requests = malloc(sizeof(t_request) * max_requests);
    if (!heap->requests)
        return (1);
    heap->request_count = 0;
    heap->max_requests = max_requests;
    return (0);
}