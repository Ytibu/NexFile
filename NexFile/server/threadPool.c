#include "threadPool.h"

int threadPoolInit(int numThreads, ThreadPool_t *threadPool)
{
    tidArrInit(numThreads, &threadPool->workerArr_); // 初始化线程信息数组
    taskQueueInit(&threadPool->ptaskQueue_); // 初始化任务队列
    
    threadPool->exitFlag_ = 0;  // 初始化退出标志
    
    pthread_mutex_init(&threadPool->mutex_, NULL);  // 初始化互斥锁
    
    pthread_cond_init(&threadPool->cond_, NULL);    // 初始化条件变量
    return 0;
}