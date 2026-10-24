#include "codexion.h"

int init_coders(t_simulation *sim)
{
    int     i;
    sim->coders = malloc(sizeof(t_coder) * sim->config.number_of_coders);
    if (!sim->coders)
        return (1);
    i = 0;
    while(i < sim->config.number_of_coders )
    {
        sim->coders[i].id = i + 1 ;
        sim->coders[i].right = &sim->dongles[i];
        sim->coders[i].left =  &sim->dongles[
            (i + sim->config.number_of_coders - 1)
            % sim->config.number_of_coders
        ];
        sim->coders[i].sim = sim;
        i++;
    }
    return 0;
}

int init_dongles(t_simulation *sim)
{
    int i;

    sim->dongles = malloc(sizeof(t_dongle)
            * sim->config.number_of_coders);
    if (!sim->dongles)
        return (1);
    i = 0;
    while (i < sim->config.number_of_coders)
    {
        sim->dongles[i].id = i + 1;
        sim->dongles[i].available = 1;
        sim->dongles[i].cooldown_until = 0;
        if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
            break ;
        if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
        {
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            break ;
        }
        if (init_heap(&sim->dongles[i].heap,
                sim->config.number_of_coders) != 0)
        {
            pthread_cond_destroy(&sim->dongles[i].cond);
            pthread_mutex_destroy(&sim->dongles[i].mutex);
            break ;
        }
        i++;
    }
    if (i != sim->config.number_of_coders)
    {
        while (--i >= 0)
        {
            destroy_heap(&sim->dongles[i].heap);
            pthread_cond_destroy(&sim->dongles[i].cond);
            pthread_mutex_destroy(&sim->dongles[i].mutex);
        }
        free(sim->dongles);
        sim->dongles = NULL;
        return (1);
    }
    return (0);
}