#ifndef CODEXION_H
# define CODEXION_H
#include <limits.h>
# include <pthread.h>

#include <stdio.h>
# include <stdlib.h>

// # data parsed
typedef struct s_data
{
    int number_of_coders;
    int time_to_burnout;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int number_of_compiles_required;
    int dongle_cooldown;
    char *scheduler;
}   t_data;

// ## dongles
typedef struct s_dongle
{
    int id;
}   t_dongle;

// ## coders
typedef struct s_coder
{
    int id;
    struct s_dongle     *left;
    struct s_dongle     *right;
    pthread_t           thread;
    struct s_simulation *sim;
}   t_coder;

// ## simulation
typedef struct s_simulation
{
    t_data    config;
    t_coder     *coders;
    t_dongle    *dongles;
    int         stop;
}   t_simulation;

int parse_arguments(int argc , char **argv, t_data *data);
int is_valid_number(char *s);
int convert_number(char *str, int *result);
int str_cmp(const char *s1, const char *s2);
int init_coders(t_simulation *sim);
int init_dongles(t_simulation *sim);

#endif