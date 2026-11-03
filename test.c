#include <stdio.h>
#include <pthread.h>

typedef struct s_data
{
    int counter ;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

} t_data ;


void *coder_routine(void *arg)
{
    t_data *data;

    data = (t_data *)arg;

    int i = 0 ;

    pthread_mutex_lock(&data->mutex);
    while (data->counter < 1000000)
    {
        pthread_cond_wait(&data->cond, &data->mutex);
    }

    // while (i < 1000000)
    // {
    //     data->counter++ ;
    //     i++ ;
    // }
    pthread_mutex_unlock(&data->mutex);
    return NULL ;
}




int main()
{
    t_data data ;

    data.counter = 0;
    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond, NULL);
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, coder_routine, &data) ;
    pthread_create(&thread2, NULL, coder_routine, &data) ;
    pthread_join(thread1,NULL);
    pthread_join(thread2, NULL);
    printf("%i" , data.counter);
    return 0 ;
}

// a process is an environment where our program executes
// and that environment has its own ressource such as virtual memory 
// , file descriptor , process id  , and at lest one thread (main thread)