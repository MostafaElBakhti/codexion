/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/15 01:05:30 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/15 01:05:35 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <stdlib.h>

static int fill_numbers(char **argv, t_struct *data)
{
    if (convert_number(argv[1], &data->number_of_coders))
        return (1);
    if (convert_number(argv[2], &data->time_to_burnout))
        return (1);
    if (convert_number(argv[3], &data->time_to_compile))
        return (1);
    if (convert_number(argv[4], &data->time_to_debug))
        return (1);
    if (convert_number(argv[5], &data->time_to_refactor))
        return (1);
    if (convert_number(argv[6], &data->number_of_compiles_required))
        return (1);
    if (convert_number(argv[7], &data->dongle_cooldown))
        return (1);
    return (0);
}

int str_cmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

int convert_number(char *str, int *result)
{
    int i;
    int number;
    int digit;

    if (!str || !result || !str[0])
        return (1);
    i = 0;
    number = 0;
    while(str[i])
    {
        digit = str[i] - '0';
        if (number > (INT_MAX - digit) / 10)
            return (1);
        number = number * 10 + digit;
        i++;
    }
    *result = number;
    return 0;
}

int is_valid_number(char *s)
{
    if(!s || !s[0])
        return 1;
    int i = 0;
    while (s[i])
    {
        if (s[i] < '0' || s[i] > '9')
            return (1);
        i++;
    }
    return 0;
}


int parse_arguments(int argc, char **argv, t_struct *data)
{
    if ( !argv || !data)
        return 1;
    int i = 1;
    while(i < argc - 1){
        if (is_valid_number(argv[i]) != 0)
            return 1;
        i++;
    }
    if (fill_numbers(argv, data) != 0)
        return 1;

    if (str_cmp(argv[8], "fifo") != 0 && str_cmp(argv[8], "edf") != 0)
        return 1;
    data->scheduler = argv[8];
    return 0;
}