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