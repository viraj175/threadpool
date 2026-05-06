#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 10

typedef struct {
    void (*fn)(void *);
    void *arg;
} task_t;

typedef struct {
    pthread_t           *threads;
    task_t              *queue;
    int                 head;
    int                 tail;
    size_t              count;
    size_t              capacity;
    pthread_mutex_t     lock;
    pthread_cond_t      not_empty;
    pthread_cond_t      not_full;
    int                 thread_count;
    int                 shutdown;
} threadpool_t;

threadpool_t *pool_create(size_t n);
void *worker(void *arg);
void task(void *arg);
void *pool_submit(threadpool_t *pool, void (*fn)(void *), void *arg);
void pool_destroy(threadpool_t *pool);

#endif // !THREADPOOL_H
