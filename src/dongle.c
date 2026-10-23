#include "codexion.h"

int try_acquire_dongle(t_dongle *dongle)
{
    int acquired;

    acquired = 0;
    pthread_mutex_lock(&dongle->mutex);
    if (dongle->available == 1)
    {
        dongle->available = 0;
        acquired = 1;
    }
    pthread_mutex_unlock(&dongle->mutex);
    return (acquired);
}

int release_dongle(t_dongle *dongle, t_simulation *sim)
{
    long now;

    now = get_time_ms()
    if (now < 0)
        return (1);
    if (pthread_mutex_lock(&dongle->mutex) != 0 )
        return (1);
    dongle->available = 1;
    dongle->cooldown_until = now + sim->config.dongle_cooldown;
    if (pthread_mutex_unlock(&dongle->mutex)!= 0 )
        return (1);
    return (0);

}

int try_acquire_dongle(t_dongle *dongle)
{
    long    now;
    int     acquired;

    acquired = 0;
    if (pthread_mutex_lock(&dongle->mutex) != 0)
        return (-1);
    now = get_time_ms();
    if (now >= 0 && dongle->available == 1
        && now >= dongle->cooldown_until)
    {
        dongle->available = 0;
        acquired = 1;
    }
    if (pthread_mutex_unlock(&dongle->mutex) != 0)
        return (-1);
    if (now < 0)
        return (-1);
    return (acquired);
}

void destroy_dongles(t_simulation *sim)
{
    int i;

    if (!sim->dongles)
        return ;
    i = 0;
    while (i < sim->config.number_of_coders)
    {
        destroy_heap(&sim->dongles[i].heap);
        pthread_mutex_destroy(&sim->dongles[i].mutex);
        i++;
    }
    free(sim->dongles);
    sim->dongles = NULL;
}