#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder     *coder;
    t_dongle    *dongle;
    t_request   request;
    int         result;
    int         can_take;
    long        now;
    t_request   winner;

    coder = (t_coder *)arg;
    dongle = coder->right;

    request.coder_id = coder->id;
    request.request_time = get_time_ms();
    if (request.request_time < 0)
        return (NULL);
    request.deadline = coder->last_compile_start
        + coder->sim->config.time_to_burnout;

    if (pthread_mutex_lock(&dongle->mutex) != 0)
        return (NULL);

    result = heap_push(&dongle->heap, request,
            coder->sim->config.scheduler);
    if (result == 0)
    {
        while (dongle->available == 0)
        {
            if (pthread_cond_wait(&dongle->cond,
                    &dongle->mutex) != 0)
            {
                pthread_mutex_unlock(&dongle->mutex);
                return (NULL);
            }
        }
    }
    can_take = 0;
    if (result == 0)
    {
        now = get_time_ms();
        if (now >= 0
            && dongle->heap.requests[0].coder_id == coder->id
            && dongle->available == 1
            && now >= dongle->cooldown_until)

            can_take = 1;
    }
    if (can_take)
    {
        if (heap_pop(&dongle->heap, &winner,
                coder->sim->config.scheduler) != 0)
            can_take = 0;
        else
            dongle->available = 0;
    }
    pthread_cond_broadcast(&dongle->cond);

    if (pthread_mutex_unlock(&dongle->mutex) != 0)
        return (NULL);

    if (result != 0)
        return (NULL);

    if (can_take)
    {
        printf("Coder %d acquired Dongle %d\n",
            coder->id, dongle->id);

        if (release_dongle(dongle, coder->sim) != 0)
            return (NULL);

        printf("Coder %d released Dongle %d\n",
            coder->id, dongle->id);
    }
    return (NULL);
}

int start_threads(t_simulation *sim)
{
    int i;
    int j;

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        sim->coders[i].last_compile_start = sim->start_time;
        i++;
    }

    i = 0;
    while (i < sim->config.number_of_coders)
    {
        if (pthread_create(&sim->coders[i].thread, NULL,
                coder_routine, &sim->coders[i]) != 0)
            break ;
        i++;
    }
    j = 0;
    while (j < i)
    {
        pthread_join(sim->coders[j].thread, NULL);
        j++;
    }
    if (i != sim->config.number_of_coders)
        return (1);
    return (0);
}

// # Mutex