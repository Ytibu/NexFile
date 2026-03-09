#include "../../shared/logger.h"
#include "../include/cmdCheck.h"
#include "../include/sendCmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

// ./client 127.0.0.1 1234
int main(int argc, char *argv[])
{
    ARGC_CHECK(argc, 3, "Usage: ./client [IP_ADDRESS] [PORT]");

    // SYSLOG("Client started with IP: %s, PORT: %s", argv[1], argv[2]);

    // 创建通信地址
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(argv[2]));
    client_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // 创建套接字并连接服务器
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sockfd, -1, "socket");
    int ret = connect(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    ERROR_CHECK(ret, -1, "connect");

    // 开始epoll事件循环
    // SYSLOG("Connected to server at %s:%s", argv[1], argv[2]);
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");

    while (1)
    {
        struct epoll_event events[1024];
        int nready = epoll_wait(epfd, events, 1024, -1);
        ERROR_CHECK(nready, -1, "epoll_wait");

        for (int i = 0; i < nready; ++i)
        {
            if (events[i].data.fd == sockfd)
            {
                char buffer[1024] = {0};
                ssize_t rev_ret = recv(sockfd, buffer, sizeof(buffer), 0);
                if (rev_ret > 0)
                {
                    printf("Received from server: %s", buffer);
                }
                else if (rev_ret == 0)
                {
                    printf("Server closed the connection.\n");
                    exit(0);
                }
                else
                {
                    perror("recv");
                    exit(0);
                }
            }
            else if (events[i].data.fd == STDIN_FILENO)
            {
                char buf[1024] = {0};
                int read_ret = read(STDIN_FILENO, buf, sizeof(buf)-1);
                if (read_ret > 0)
                {
                    buf[read_ret-1] = '\0';          // read 不会自动补 \0
                    char cmd[1024];
                    PacketCmd_t pcmdArg;
                    memcpy(cmd, buf, read_ret + 1);
                    int check_ret = cmdCheck(cmd, &pcmdArg);
                    if(check_ret == 0){
                        printf("%s: command not found\n", cmd);
                        goto end;
                    }
                    sendCmd(sockfd, &pcmdArg);
                    printf("Sent command to server, cmd:%d, arg:%s, length:%d \n", pcmdArg.cmdCode_, pcmdArg.data_, pcmdArg.length_);
                    // ssize_t send_ret = send(sockfd, buf, strlen(buf), 0);
                    // ERROR_CHECK(send_ret, -1, "send");
                    
                }
                else if (read_ret == 0)
                {
                    printf("End of input detected. Exiting.\n");
                    exit(0);
                }
                else
                {
                    perror("read");
                    exit(0);
                }
            }
        }
        end:
        
    }

    close(sockfd);
    close(epfd);

    return 0;
}