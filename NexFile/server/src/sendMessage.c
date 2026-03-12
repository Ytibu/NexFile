#include "../include/sendMessage.h"
#include "../include/authen.h"
#include "../../shared/protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

// 一次性读取指定长度的数据
int recvn(int sockFd, void *buf, long total)
{
    char *p = (char *)buf;
    long cursize = 0;
    while (cursize < total)
    {
        ssize_t sret = recv(sockFd, p + cursize, total - cursize, 0);
        if (sret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        if (sret == 0)
        {
            return 0;
        }
        cursize += sret;
    }
    return (int)cursize;
}

// 接收认证信息并校验密码
int UserAuthen(int sockfd)
{
    train_t train;
    const int trainDataCap = sizeof(train.data_);
    char username[sizeof(train.data_) + 1] = {0};
    int ret = recvn(sockfd, &train.length_, sizeof(train.length_));
    if (ret <= 0)
    {
        return -1;
    }
    if (train.length_ < 0 || train.length_ > trainDataCap)
    {
        errno = EMSGSIZE;
        return -1;
    }
    ret = recvn(sockfd, train.data_, train.length_);
    if (ret <= 0)
    {
        return -1;
    }
    memcpy(username, train.data_, train.length_);
    username[train.length_] = '\0';


    char cryptPassword[sizeof(train.data_) + 1] = {0};
    ret = recvn(sockfd, &train.length_, sizeof(train.length_));
    if (ret <= 0)
    {
        return -1;
    }
    if (train.length_ < 0 || train.length_ > trainDataCap)
    {
        errno = EMSGSIZE;
        return -1;
    }
    ret = recvn(sockfd, train.data_, train.length_);
    if (ret <= 0)
    {
        return -1;
    }
    memcpy(cryptPassword, train.data_, train.length_);
    cryptPassword[train.length_] = '\0';


    int auth_ret = authenticateUser(username, cryptPassword);
    if(auth_ret == 1)
    {
        const char *success_msg = "AUTH_SUCCESS";
        send(sockfd, success_msg, strlen(success_msg), 0);
        return 1;
    }

    if (auth_ret == 0)
    {
        const char *fail_msg = "AUTH_FAIL";
        send(sockfd, fail_msg, strlen(fail_msg), 0);
        return 0;
    }

    return -1;
}
