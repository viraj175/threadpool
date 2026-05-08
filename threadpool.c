#include "threadpool.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

threadpool_t*
pool_create(size_t n)
{
    threadpool_t *pool = malloc(sizeof(threadpool_t));
    if (!pool)
    {
        perror("malloc failed");
        return NULL;
    }

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);

    pool->threads = malloc(n * sizeof(pthread_t));
    pool->queue = malloc(CAPACITY * sizeof(task_t));
    if (!pool->threads || !pool->queue)
    {
        perror("Malloc failed");
        return NULL;
    }
    pool->front = pool->rear = 0;
    pool->count = 0;
    pool->thread_count = n;
    pool->capacity = CAPACITY;
    pool->shutdown = 0;

    for (size_t i = 0; i < n; ++i)
    {
        if(pthread_create(&pool->threads[i], NULL, worker, pool) != 0)
        {
            perror("pthread_create() failed");
            pool->thread_count = i;
            pool_destroy(&pool);
            return NULL;
        }
    }

    return pool;
}

void*
worker(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;
    while (1)
    {
        pthread_mutex_lock(&pool->lock);
        while(pool->count == 0 && !pool->shutdown)
            pthread_cond_wait(&pool->not_empty, &pool->lock);
        if (pool->count == 0 && pool->shutdown)
        {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }
        task_t task = pool->queue[pool->front];
        pool->front = (pool->front + 1) % pool->capacity;
        pool->count--;
        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->lock);

        task.fn(task.arg);
    }
       
    return NULL;
}

void*
pool_submit(threadpool_t *pool, void (*fn)(void *), void *arg)
{
    pthread_mutex_lock(&pool->lock);
    while (pool->count >= pool->capacity)
        pthread_cond_wait(&pool->not_full, &pool->lock);
    pool->rear = (pool->count + pool->front) % pool->capacity;
    pool->queue[pool->rear].fn = fn;
    pool->queue[pool->rear].arg = arg;
    pool->count++;
    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->lock);
    return NULL;
}

void
task(void *arg)
{
    printf("%lu thread ran the task!\n", pthread_self());
}

void
pool_destroy(threadpool_t **pool)
{
    if (!pool || !*pool)
    {
        fprintf(stderr, "No pool provided\n");
        return;
    }
    
    threadpool_t *delete = *pool;
    pthread_mutex_lock(&delete->lock);
    delete->shutdown = 1;
    pthread_cond_broadcast(&delete->not_empty);
    pthread_mutex_unlock(&delete->lock);

    for (size_t i = 0; i < delete->thread_count; ++i)
        pthread_join(delete->threads[i], NULL);

    pthread_mutex_destroy(&delete->lock);
    pthread_cond_destroy(&delete->not_empty);
    pthread_cond_destroy(&delete->not_full);

    free(delete->threads);
    free(delete->queue);

    free(*pool);
    *pool = NULL;
}
