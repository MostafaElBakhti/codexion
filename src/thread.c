#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    printf("Hello from coder %d\n", coder->id);
    sleep(3);
    printf("Coder %d finished\n", coder->id);
    return (NULL);
}

int start_threads(t_simulation *sim)
{
    int i;
    int j;

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