#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H

#include <pthread.h>
#include "worker.h"
#include "taskQueue.h"

// 线程池结构体
typedef struct ThreadPool_s {
    workerArr_t workerArr_; // 线程信息数组
    pthread_mutex_t mutex_; // 互斥锁
    pthread_cond_t cond_; // 条件变量
    TaskQueue_t ptaskQueue_; // 任务队列
    int exitFlag_; // 退出标志
} ThreadPool_t;

int threadPoolInit(int workerNum, ThreadPool_t *threadPool);

#endif