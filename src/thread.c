#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *coder;
    long    deadline;

    coder = (t_coder *)arg;
    deadline = coder->last_compile_start
        + coder->sim->config.time_to_burnout;

    printf("Coder %d deadline: %ld ms\n",
        coder->id, deadline - coder->sim->start_time);

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