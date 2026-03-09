#include "../include/server/worker.h"

#include "../include/logger.h"
#include "../include/server/threadPool.h"
#include "../include/server/epoll.h"
#include "../include/server/recvCmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/epoll.h>

int tidArrInit(int workerNum, workerArr_t *pworkerArr)
{
    pworkerArr->threadId_ = (pthread_t *)calloc(workerNum, sizeof(pthread_t));
    ERROR_CHECK(pworkerArr->threadId_, NULL, "calloc");
    pworkerArr->workerNum_ = workerNum;
    return 0;
}

// 创建多线程
int makeWorker(ThreadPool_t *pthreadPool)
{
    for (int i = 0; i < pthreadPool->workerArr_.workerNum_; ++i)
    {
        int cre_ret = pthread_create(&pthreadPool->workerArr_.threadId_[i], NULL, workerFunc, (void *)pthreadPool);
        ERROR_CHECK(cre_ret, -1, "pthread_create");
        printf("Worker thread %d created, thread ID: %lu\n", i, pthreadPool->workerArr_.threadId_[i]);
    }

    return 0;
}

void *workerFunc(void *arg)
{
    ThreadPool_t *threadPool = (ThreadPool_t *)arg;
    (void)threadPool;
    while (1)
    {
        // 线程池退出标志
        pthread_mutex_lock(&threadPool->mutex_);
        while (threadPool->exitFlag_ == 0 && threadPool->ptaskQueue_.queueSize_ <= 0)
        {
            pthread_cond_wait(&threadPool->cond_, &threadPool->mutex_);
        }
        if (threadPool->exitFlag_ == 1)
        {
            pthread_mutex_unlock(&threadPool->mutex_);
            printf("Worker thread %lu exiting...\n", pthread_self());
            pthread_exit(NULL);
        }

        // 线程等待条件变量
        int clientFd = threadPool->ptaskQueue_.phead_->netFd; // 从任务队列头部获取客户端文件描述符
        taskQueuePop(&threadPool->ptaskQueue_);               // 从任务队列中取出一个任务
        pthread_mutex_unlock(&threadPool->mutex_);

        // 处理任务
        printf("Worker thread %lu processing client FD: %d\n", pthread_self(), clientFd);
        // 这里可以添加具体的任务处理逻辑，例如读取客户端请求、处理数据、发送响应等

        int epfd = epoll_create(1); // 将客户端文件描述符添加到epoll实例中，等待事件发生
        epollADD(epfd, clientFd);   // 使用边缘触发模式监听客户端文件描述符的可读事件

        while (1)
        {
            struct epoll_event readySet[1024];
            int readyNum = epoll_wait(epfd, readySet, 1024, -1); // 等待事件发生

            for (int i = 0; i < readyNum; ++i)
            {
                if (readySet[i].data.fd == clientFd)
                { // 可读事件
                    // 处理可读事件，例如读取客户端请求数据
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    PacketCmd_t header;
                    int ret = recvCmd(clientFd, &header);
                    // 处理接收到的命令，例如根据header.cmdCode_执行相应的操作
                    if (ret > 0)
                    {
                        printf("Received command code: %u, data length: %s\n", header.cmdCode_, header.data_);
                    }
                    else if (ret == 0)
                    {
                        printf("Client FD %d closed the connection.\n", clientFd);
                        close(clientFd); // 关闭客户端文件描述符
                        break;           // 退出循环，等待下一个任务
                    }
                    else
                    {
                        perror("recvCmd");
                        close(clientFd); // 关闭客户端文件描述符
                        break;           // 退出循环，等待下一个任务
                    }
                }
            }
        }
    }

    return NULL;
}