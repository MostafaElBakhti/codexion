#ifndef CODEXION_H
# define CODEXION_H
#include <limits.h>
#include <unistd.h>
# include <pthread.h>
#include <string.h>

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

typedef struct s_request
{
    int     coder_id;
    long    request_time;
    long    deadline;
}   t_request;


// ## heap
typedef struct s_heap
{
    t_request   *requests;
    int         request_count;
    int         max_requests;
}   t_heap;

// ## dongles
typedef struct s_dongle
{
    int             id;
    int             available;
    long            cooldown_until;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          heap;
}   t_dongle;

// ## coders
typedef struct s_coder
{
    int                 id;
    struct s_dongle     *left;
    struct s_dongle     *right;
    pthread_t           thread;
    struct s_simulation *sim;
    long                last_compile_start;
}   t_coder;

// ## simulation
typedef struct s_simulation
{
    t_data      config;
    t_coder     *coders;
    t_dongle    *dongles;
    int         stop;
    long        start_time;
}   t_simulation;



int     parse_arguments(int argc , char **argv, t_data *data);
int     is_valid_number(char *s);
int     convert_number(char *str, int *result);
int     str_cmp(const char *s1, const char *s2);
int     init_coders(t_simulation *sim);
int     init_dongles(t_simulation *sim);
void    *coder_routine(void *arg);
int     try_acquire_dongle(t_dongle *dongle);
long    get_time_ms(void);
int     release_dongle(t_dongle *dongle, t_simulation *sim);
int     init_heap(t_heap *heap, int max_requests);
void    destroy_heap(t_heap *heap);
int     heap_push(t_heap *heap, t_request request, char *scheduler);
int     start_threads(t_simulation *sim);
void    destroy_dongles(t_simulation *sim);

#endif