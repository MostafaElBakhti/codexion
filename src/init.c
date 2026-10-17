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
        i++;
    }
    return (0);
}