#include <sched.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "threadpool.h"

int
main()
{
    threadpool_t *pool = pool_create(5);

    for (int i = 0; i < pool->count; ++i)
        pthread_join(pool->threads[i], NULL);

    pool_destroy(pool);   

    return 0;
}
