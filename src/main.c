/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 01:05:49 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/15 01:05:52 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(int argc , char **argv)
{
    t_simulation    sim;

    if (argc != 9)
    {
        printf("Error: Expected 8 arguments\n");
        return(1);
    }
    
    if (parse_arguments(argc, argv, &sim.config) != 0)
    {
        printf("Error: Invalid arguments\n");
        return (1);
    }
    // printf("Parsing successful!\n");
    // printf("number_of_coders: %d\n", data.number_of_coders);
    // printf("time_to_burnout: %d\n", data.time_to_burnout);
    // printf("time_to_compile: %d\n", data.time_to_compile);
    // printf("time_to_debug: %d\n", data.time_to_debug);
    // printf("time_to_refactor: %d\n", data.time_to_refactor);
    // printf("number_of_compiles_required: %d\n", data.number_of_compiles_required);
    // printf("dongle_cooldown: %d\n", data.dongle_cooldown);
    
    sim.stop = 0;
    if (init_dongles(&sim) != 0)
        return (1);
    if (init_coders(&sim) != 0)
    {
        free(sim.dongles);
        return (1);
    }
    
    
    free(sim.coders);
    free(sim.dongles);
    return (0);

}