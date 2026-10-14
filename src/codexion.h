#ifndef CODEXION_H
# define CODEXION_H
#include <limits.h>

#include <stdio.h>


typedef struct s_struct
{
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char *scheduler;
}   t_struct;


int parse_arguments(int argc , char **argv, t_struct *data);
int is_valid_number(char *s);
int convert_number(char *str, int *result);
static int fill_numbers(char **argv, t_struct *data);
int str_cmp(const char *s1, const char *s2);

#endif