#include <sched.h>
// #include <stdio.h>
// #include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "threadpool.h"

int
main()
{
    threadpool_t *pool = pool_create(5);

    for (int i = 0; i < 8; i++)
        pool_submit(pool, task, NULL);

    pool_destroy(&pool);   

    return 0;
}
