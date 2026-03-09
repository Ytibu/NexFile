#ifndef __WORKER_H__
#define __WORKER_H__

#include <pthread.h>

typedef struct ThreadPool_s ThreadPool_t; // 前置声明

// 工作线程信息结构体
typedef struct workerArr_s {
    pthread_t *threadId_;
    int workerNum_;
} workerArr_t;

int tidArrInit(int workerNum, workerArr_t *pworkerArr);

// 创建多线程
int makeWorker(ThreadPool_t *pthreadPool);

void *workerFunc(void *arg);

#endif