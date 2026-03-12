#include "../../shared/logger.h"
#include "../include/cmdCheck.h"
#include "../include/sendCmd.h"
#include "../include/status.h"
#include "../include/encryption.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

// 封装epoll_ctl添加事件的函数
static void add_epoll_in(int epfd, int fd)
{
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    ERROR_CHECK(ret, -1, "epoll_ctl");
}

// 处理来自服务器的消息
static int handle_socket_event(int sockfd)
{
    char buffer[1024];
    ssize_t recv_ret = recv(sockfd, buffer, sizeof(buffer), 0);
    if (recv_ret > 0)
    {
        printf("Received from server: %.*s", (int)recv_ret, buffer);
        return 0;
    }
    if (recv_ret == 0)
    {
        printf("Server closed the connection.\n");
        return -1;
    }

    perror("recv");
    return -1;
}

// 处理来自标准输入的命令
static int handle_stdin_event(int sockfd)
{
    char buf[1024] = {0};
    ssize_t read_ret = read(STDIN_FILENO, buf, sizeof(buf) - 1);
    if (read_ret < 0)
    {
        perror("read");
        return -1;
    }
    else if (read_ret == 0)
    {
        printf("End of input detected. Exiting.\n");
        return -1;
    }

    buf[read_ret] = '\0';

    if (buf[read_ret - 1] == '\n')
    {
        buf[read_ret - 1] = '\0';
    }

    char cmd[1024] = {0};
    memcpy(cmd, buf, strlen(buf));

    packetCmd_t pcmdArg;
    int check_ret = cmdCheck(cmd, &pcmdArg);
    if (check_ret == 0)
    {
        printf("%s: command not found\n", cmd);
        return 0;
    }

    sendCmd(sockfd, &pcmdArg);

    if (pcmdArg.argFlag_ == 1)
    {
        printf("Parsed command: cmdCode=%d, argFlag=%d, length=%d, data=%s\n",
               pcmdArg.cmdCode_, pcmdArg.argFlag_, pcmdArg.length_, pcmdArg.data_);
    }
    else
    {
        printf("Parsed command: cmdCode=%d, argFlag=%d\n",
               pcmdArg.cmdCode_, pcmdArg.argFlag_);
    }
    return 0;
}

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

    add_epoll_in(epfd, STDIN_FILENO);
    add_epoll_in(epfd, sockfd);

    // 直接要求认证并直接校验处理
    char username[64] = {0};
    char password[128] = {0};

    printf("启动用户校验：\n");
    while (g_clientState.is_connected != 1) //查看认证状态
    {
        printf("输入用户名Username: ");
        if (fgets(username, sizeof(username), stdin) == NULL)
        {
            printf("Failed to read username. Exiting.\n");
            continue;
        }
        printf("输入密码Password: ");
        if (fgets(password, sizeof(password), stdin) == NULL)
        {
            printf("Failed to read password. Exiting.\n");
            continue;
        }

        // Remove trailing newline characters
        username[strcspn(username, "\n")] = '\0';
        password[strcspn(password, "\n")] = '\0';

        if (handle_authentication(sockfd, username, password) != 0)
        {
            printf("用户校验失败，重新输入\n");
            continue;
        }

        if(g_clientState.is_connected == 1)
        {
            printf("成功登录,欢迎： %s!\n", g_clientState.username);
            break;
        }
        
    }

    int should_exit = 0;
    while (1)
    {
        struct epoll_event events[1024];
        int nready = epoll_wait(epfd, events, 1024, -1);
        if (nready == -1)
        {
            perror("epoll_wait");
            should_exit = 1;
            break;
        }

        for (int i = 0; i < nready; ++i)
        {
            if (events[i].data.fd == sockfd)
            {
                if (handle_socket_event(sockfd) != 0)
                {
                    should_exit = 1;
                    break;
                }
            }
            else if (events[i].data.fd == STDIN_FILENO)
            {
                if (handle_stdin_event(sockfd) != 0)
                {
                    should_exit = 1;
                    break;
                }
            }
        }

        if (should_exit)
        {
            break;
        }
    }

    close(sockfd);
    close(epfd);

    return 0;
}