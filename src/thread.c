#include "codexion.h"

void *coder_routine(void *arg)
{
    t_coder *coder;

    coder = (t_coder *)arg;
    printf("Hello from coder %d\n", coder->id);
    return (NULL);
}