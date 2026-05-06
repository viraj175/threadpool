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
    if (!pool)
    {
        pthread_mutex_destroy(&pool->lock);
        pthread_cond_destroy(&pool->not_empty);
        pthread_cond_destroy(&pool->not_full);
        return -1;
    }

    for (int i = 0; i < pool->count; ++i)
        pthread_join(pool->threads[i], NULL);

    pool_destroy(pool);   

    return 0;
}
