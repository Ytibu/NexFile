#include "../include/worker.h"

#include "../../shared/logger.h"
#include "../include/threadPool.h"
#include "../include/epoll.h"
#include "../include/recvCmd.h"
#include "../include/sendMessage.h"

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

static void handleClient(int clientFd)
{
    int epfd = epoll_create(1);
    if (epfd < 0)
    {
        perror("epoll_create");
        close(clientFd);
        return;
    }

    if (epollADD(epfd, clientFd) < 0)
    {
        perror("epollADD");
        close(epfd);
        close(clientFd);
        return;
    }

    int authRet = UserAuthen(clientFd);
    if (authRet <= 0)
    {
        if (authRet == 0)
        {
            printf("Client FD %d authentication failed.\n", clientFd);
        }
        else
        {
            perror("UserAuthen");
        }
        close(epfd);
        close(clientFd);
        return;
    }

    while (1)
    {
        
        struct epoll_event readySet[1024];
        int readyNum = epoll_wait(epfd, readySet, 1024, -1);
        if (readyNum < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("epoll_wait");
            break;
        }

        int shouldClose = 0;
        for (int i = 0; i < readyNum; ++i)
        {
            if (readySet[i].data.fd != clientFd)
            {
                continue;
            }

            packetCmd_t header;
            int ret = recvCmd(clientFd, &header);
            if (ret > 0)
            {
                printf("Received command code: %u, data length: %s\n", header.cmdCode_, header.data_);
                continue;
            }

            if (ret == 0)
            {
                printf("Client FD %d closed the connection.\n", clientFd);
            }
            else
            {
                perror("recvCmd");
            }
            shouldClose = 1;
            break;
        }

        if (shouldClose)
        {
            break;
        }
    }

    close(epfd);
    close(clientFd);
}

void *workerFunc(void *arg)
{
    ThreadPool_t *threadPool = (ThreadPool_t *)arg;
    while (1)
    {
        pthread_mutex_lock(&threadPool->mutex_);
        while (threadPool->exitFlag_ == 0 && threadPool->ptaskQueue_.queueSize_ <= 0)
        {
            pthread_cond_wait(&threadPool->cond_, &threadPool->mutex_);
        }

        if (threadPool->exitFlag_ == 1)
        {
            pthread_mutex_unlock(&threadPool->mutex_);
            printf("Worker thread %lu exiting...\n", pthread_self());
            return NULL;
        }

        int clientFd = threadPool->ptaskQueue_.phead_->netFd;
        taskQueuePop(&threadPool->ptaskQueue_);
        pthread_mutex_unlock(&threadPool->mutex_);

        printf("Worker thread %lu processing client FD: %d\n", pthread_self(), clientFd);
        handleClient(clientFd);
    }

    return NULL;
}