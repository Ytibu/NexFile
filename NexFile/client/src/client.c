#include "../../shared/logger.h"
#include "../include/cmdOpt.h"
#include "../include/status.h"
#include "../include/encryption.h"
#include "../include/epoll.h"
#include "../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

// 处理来自服务器的消息
static int handle_socket_event(int sockfd)
{
    char buffer[1024];
    ssize_t recv_ret = recv(sockfd, buffer, sizeof(buffer), 0);
    if (recv_ret < 0)
    {
        perror("recv");
        return -1;
    }
    else if (recv_ret == 0)
    {
        printf("Server closed the connection.\n");
        return -1;
    }

    printf("Received from server: %.*s", (int)recv_ret, buffer);
    return 0;
}

// 处理来自标准输入的命令 -1表示退出，1表示命令不合法，0表示正常处理
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

    packetCmd_t pcmdArg;
    // 解析命令并检查合法性:返回-1表示命令不合法，0表示合法
    int check_ret = cmdCheck(buf, &pcmdArg);
    if (check_ret != 0)
    {
        printf("%s: command not found\n", buf);
        return 1;
    }

    // 发送命令给服务器:返回-1表示发送失败，大于0表示发送成功
    // check_ret = sendCmd(sockfd, &pcmdArg);
    // if(check_ret < 0)
    // {
    //     printf("Failed to send command to server.\n");
    //     return -1;
    // }

    if (pcmdArg.argFlag_ == 1)
    {
        LOGINFO("Parsed command: cmdCode=%d, argFlag=%d, length=%d, data=%s\n",
               pcmdArg.cmdCode_, pcmdArg.argFlag_, pcmdArg.length_, pcmdArg.data_);
    }
    else
    {
        LOGINFO("Parsed command: cmdCode=%d, argFlag=%d\n",
               pcmdArg.cmdCode_, pcmdArg.argFlag_);
    }

    handleCommand(sockfd, &pcmdArg);

    return 0;
}

// 认证阶段探测服务端是否已断开连接
static int check_auth_server_alive(int sockfd)
{
    char ch;
    ssize_t ret = recv(sockfd, &ch, 1, MSG_PEEK | MSG_DONTWAIT);
    if (ret == 0)
    {
        printf("Server closed the connection during authentication.\n");
        return -1;
    }
    if (ret < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
        {
            return 0;
        }
        perror("recv");
        return -1;
    }
    return 0;
}

// 循环处理用户输入，直到认证成功
static int handle_authen_event(int sockfd)
{
    // 直接要求认证并直接校验处理
    char username[64] = {0};
    char password[128] = {0};

    printf("启动用户校验：\n");
    while (g_clientState.is_connected != 1) // 查看认证状态
    {
        if (check_auth_server_alive(sockfd) != 0)
        {
            return -1;
        }

        printf("输入用户名Username: ");
        if (fgets(username, sizeof(username), stdin) == NULL)
        {
            printf("Input closed while waiting for username. Exiting.\n");
            return -1;
        }

        if (check_auth_server_alive(sockfd) != 0)
        {
            return -1;
        }

        printf("输入密码Password: ");
        if (fgets(password, sizeof(password), stdin) == NULL)
        {
            printf("Input closed while waiting for password. Exiting.\n");
            return -1;
        }

        username[strcspn(username, "\n")] = '\0';
        password[strcspn(password, "\n")] = '\0';

        if (check_auth_server_alive(sockfd) != 0)
        {
            return -1;
        }

        if (handle_authentication(sockfd, username, password) != 0)
        {
            if (check_auth_server_alive(sockfd) != 0)
            {
                return -1;
            }
            printf("用户校验失败，重新输入\n");
            continue;
        }
    }
    LOGINFO("User authenticated successfully as '%s'", username);
    return 0;
}

int main(int argc, char *argv[])
{
    ARGC_CHECK(argc, 2, "client: ./client [config_file_path]");
    LOGINFO("Client started with config file: %s", argv[1]);

    // 解析配置
    const Config_t *g_clientConfig = GetGlobalConfig();
    if (g_clientConfig)
    {
        printClientConfig(&g_clientConfig);
    }
    else
    {
        fprintf(stderr, "配置文件解析失败\n");
        return 1;
    }

    // 获取服务器地址信息
    struct sockaddr_in client_addr = g_clientConfig->addr;

    // 创建套接字并连接服务器
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(sockfd, -1, "socket");
    int ret = connect(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
    ERROR_CHECK(ret, -1, "connect");
    LOGINFO("Connected to server at %s:%d", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    if (handle_authen_event(sockfd) != 0) // 处理认证事件，直到成功登录
    {
        close(sockfd);
        return 1;
    }
    

    // 开始epoll事件循环
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    epollADD(epfd, STDIN_FILENO);
    epollADD(epfd, sockfd);

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
                // 返回0正常处理命令，返回1命令不合法继续等待输入，返回-1表示退出
                int handle_ret = handle_stdin_event(sockfd);
                if (handle_ret == -1)
                {
                    should_exit = 1;
                    break;
                }
                else if (handle_ret == 1)
                {
                    // 命令不合法，继续等待输入
                    continue;
                }

                // TODO: 处理命令输入后的其他逻辑，例如等待服务器响应等
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
