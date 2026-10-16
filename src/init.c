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
        i++;
    }
    return 0;
}