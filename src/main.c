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

int main(int argc, char **argv)
{
    t_simulation    sim;

    if (argc != 9)
        return (1);
    if (parse_arguments(argc, argv, &sim.config) != 0)
        return (1);
    sim.stop = 0;
    if (init_dongles(&sim) != 0)
        return (1);
    if (init_coders(&sim) != 0)
    {
        free(sim.dongles);
        return (1);
    }
    if (pthread_create(&sim.coders[0].thread, NULL,
            coder_routine, &sim.coders[0]) != 0)
    {
        free(sim.coders);
        free(sim.dongles);
        return (1);
    }
    pthread_join(sim.coders[0].thread, NULL);
    free(sim.coders);
    free(sim.dongles);
    return (0);
}