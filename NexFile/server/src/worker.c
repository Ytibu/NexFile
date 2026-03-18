#include "../include/worker.h"

#include "../../shared/logger.h"
#include "../include/threadPool.h"
#include "../include/epoll.h"
#include "../include/recvCmd.h"
#include "../include/sendMessage.h"
#include "../include/cmdHandle.h"

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

// 处理用户校验和后续命令交互的主函数
static int handle_authen_event(int clientFd)
{
    while (1)
    {
        int authRet = UserAuthen(clientFd);
        if (authRet == AUTH_RESULT_OK)
        {
            printf("Client FD %d authenticated successfully.\n", clientFd);
            return 0;
        }

        if (authRet == AUTH_RESULT_FAIL)
        {
            printf("Client FD %d authentication failed.\n", clientFd);
            continue;
        }

        if (authRet == AUTH_RESULT_USER_QUERY_FAIL)
        {
            printf("Client FD %d user information failed.\n", clientFd);
            continue;
        }

        printf("Client FD %d disconnected or I/O error during authentication.\n", clientFd);
        return -1;
    }
}

static void handleClient(int clientFd)
{
    if (handle_authen_event(clientFd) != 0) // 认证阶段连接断开或I/O异常
    {
        close(clientFd);
        return;
    }

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

            packetCmd_t cmd;
            int ret = recvCmd(clientFd, &cmd);
            if (ret == 0)
            {
                printf("Client FD %d closed the connection.\n", clientFd);
                shouldClose = 1;
                break;
            }
            else if (ret < 0)
            {
                printf("Error receiving command from client FD %d.\n", clientFd);
                shouldClose = 1;
                break;
            }

            if (cmd.argFlag_ == 1)
            {
                printf("Parsed command: cmdCode=%d, argFlag=%d, length=%d, data=%s\n",
                       cmd.cmdCode_, cmd.argFlag_, cmd.length_, cmd.data_);
            }
            else
            {
                printf("Parsed command: cmdCode=%d, argFlag=%d\n",
                       cmd.cmdCode_, cmd.argFlag_);
            }

            cmdParse(clientFd, &cmd);
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
