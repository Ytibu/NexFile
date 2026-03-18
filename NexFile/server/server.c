#include  "../shared/logger.h"
#include "tcpInit.h"
#include "epoll.h"
#include "threadPool.h"
#include "taskQueue.h"
#include "serverConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int exitPipefd[2];
void sigchld_handler(int signum)
{
    printf("Received signal: %d\n", signum);
    write(exitPipefd[1], "exit", 4);
}


int main()
{
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = SERVER_FAMILY;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    pipe(exitPipefd);
    int pid = fork();
    ERROR_CHECK(pid, -1, "fork");
    if (pid)
    {
        close(exitPipefd[0]);

        signal(SIGCHLD, SIG_IGN); // 避免僵尸进程
        wait(NULL);
        exit(0);
    }
    close(exitPipefd[1]);

    ThreadPool_t threadPool;
    threadPoolInit(WORKER_THREAD_NUM, &threadPool); //  线程池初始化
    makeWorker(&threadPool);                       // 创建多线程

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sockFd, -1, "socket");

    int reuse = 1;
    int ret = setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    ERROR_CHECK(ret, -1, "setsockopt");

    ret = bind(sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    ERROR_CHECK(ret, -1, "bind");

    ret = listen(sockFd, 5);
    ERROR_CHECK(ret, -1, "listen");

    int epFd = epoll_create1(0);
    epollADD(epFd, sockFd);
    epollADD(epFd, exitPipefd[0]);

    while (1)
    {
        struct epoll_event events[1024];
        int readyNum = epoll_wait(epFd, events, 1024, -1);

        for (int i = 0; i < readyNum; ++i)
        {
            if (events[i].data.fd == sockFd)
            {
                int clientFd = accept(sockFd, NULL, NULL);
                if (clientFd < 0)
                {
                    perror("accept");
                    continue;
                }
                epollADD(epFd, clientFd);
                printf("new client connected, fd: %d\n", clientFd);
                pthread_mutex_lock(&threadPool.mutex_);
                taskQueuePush(&threadPool.ptaskQueue_, clientFd);
                pthread_cond_signal(&threadPool.cond_);
                pthread_mutex_unlock(&threadPool.mutex_);
            }
            else if (events[i].data.fd == exitPipefd[0])
            {
                char buf[5] = {0};
                read(exitPipefd[0], buf, sizeof(buf));
                pthread_mutex_lock(&threadPool.mutex_);
                threadPool.exitFlag_ = 1;
                pthread_cond_broadcast(&threadPool.cond_);
                pthread_mutex_unlock(&threadPool.mutex_);

                for (int j = 0; j < threadPool.workerArr_.workerNum_; ++j)
                {
                    pthread_join(threadPool.workerArr_.threadId_[j], NULL); /*等待子线程退出*/
                }
                printf("all worker thread exited, exiting main thread...\n");
                close(sockFd);
                close(exitPipefd[0]);

                close(epFd);
                return 0;
            }
        }
    }

    close(sockFd);
    close(exitPipefd[0]);
    close(epFd);

    return 0;
}