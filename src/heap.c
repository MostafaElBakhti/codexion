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

void destroy_heap(t_heap *heap)
{
    if (!heap)
        return ;
    free(heap->requests);
    heap->requests = NULL;
    heap->request_count = 0;
    heap->max_requests = 0;
}

static int higher_priority(t_request a, t_request b, char *scheduler)
{
    if (strcmp(scheduler, "fifo") == 0)
    {
        if (a.request_time != b.request_time)
            return (a.request_time < b.request_time);
    }
    else
    {
        if (a.deadline != b.deadline)
            return (a.deadline < b.deadline);
        if (a.request_time != b.request_time)
            return (a.request_time < b.request_time);
    }
    return (a.coder_id < b.coder_id);
}

int heap_push(t_heap *heap, t_request request, char *scheduler)
{
    int         i;
    int         parent;
    t_request   temp;

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

int heap_pop(t_heap *heap, t_request *request, char *scheduler)
{
    int         i;
    int         left;
    int         right;
    int         smallest;
    t_request   temp;

    if (!heap || !request || heap->request_count == 0)
        return (1);
    *request = heap->requests[0];
    heap->request_count--;
    if (heap->request_count == 0)
        return (0);
    heap->requests[0] = heap->requests[heap->request_count];

    i = 0;
    while (1)
    {
        left = 2 * i + 1;
        right = 2 * i + 2;
        smallest = i;
        if (left < heap->request_count
            && higher_priority(heap->requests[left],
                heap->requests[smallest], scheduler))
            smallest = left;
        if (right < heap->request_count
            && higher_priority(heap->requests[right],
                heap->requests[smallest], scheduler))
            smallest = right;
        if (smallest == i)
            break ;
        temp = heap->requests[i];
        heap->requests[i] = heap->requests[smallest];
        heap->requests[smallest] = temp;
        i = smallest;
    }
    return (0);
}