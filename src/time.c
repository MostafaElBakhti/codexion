#include "codexion.h"
#include <time.h>

long get_time_ms(void)
{
    // struct timespec
    // {
    //     time_t tv_sec;   // Seconds
    //     long   tv_nsec;  // Nanoseconds
    // };
    struct timespec ts;

    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0 )
        return (-1);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}