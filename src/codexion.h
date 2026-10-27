/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mel-bakh <mel-bakh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 00:00:00 by mel-bakh          #+#    #+#             */
/*   Updated: 2026/10/27 00:00:00 by mel-bakh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <limits.h>
# include <unistd.h>
# include <pthread.h>
# include <time.h>
# include <string.h>

# include <stdio.h>
# include <stdlib.h>

typedef struct s_data
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	*scheduler;
}	t_data;

typedef struct s_request
{
	int		coder_id;
	long	request_time;
	long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			request_count;
	int			max_requests;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	int				available;
	long			cooldown_until;
	pthread_mutex_t	mutex;
	t_heap			heap;
}	t_dongle;

typedef struct s_coder
{
	int					id;
	struct s_dongle		*left;
	struct s_dongle		*right;
	pthread_t			thread;
	struct s_simulation	*sim;
	long				last_compile_start;
}	t_coder;

typedef struct s_simulation
{
	t_data			config;
	t_dongle		*dongles;
	t_coder			*coders;
	int				stop;
	long			start_time;
	pthread_mutex_t	arbiter_mutex;
	pthread_cond_t	arbiter_cond;
	pthread_mutex_t	log_mutex;
}	t_simulation;

int		parse_arguments(int argc, char **argv, t_data *data);
int		is_valid_number(char *s);
int		convert_number(char *str, int *result);
int		init_coders(t_simulation *sim);
int		init_dongles(t_simulation *sim);
void	*coder_routine(void *arg);
long	get_time_ms(void);
int		acquire_pair(t_coder *coder);
int		release_pair(t_coder *coder);
int		queue_pair(t_coder *coder, t_request request);
int		reserve_pair(t_coder *coder, long now);
void	cancel_pair(t_coder *coder);
void	lock_pair(t_coder *coder);
void	unlock_pair(t_coder *coder);
void	stop_simulation(t_simulation *sim);
int		wait_until(t_simulation *sim, long deadline);
void	log_state(t_coder *coder, char *state);
int		init_heap(t_heap *heap, int max_requests);
void	destroy_heap(t_heap *heap);
int		heap_push(t_heap *heap, t_request request, char *scheduler);
int		start_threads(t_simulation *sim);
void	destroy_dongles(t_simulation *sim);
int		heap_pop(t_heap *heap, t_request *request, char *scheduler);
int		heap_remove(t_heap *heap, int coder_id, char *scheduler);
int		higher_priority(t_request a, t_request b, char *scheduler);
void	heap_repair(t_heap *heap, int i, char *scheduler);

#endif
