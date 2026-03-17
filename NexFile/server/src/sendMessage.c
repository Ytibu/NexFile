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

int sendn(int sockfd, const void *buf, long total)
{
    const char *p = (const char *)buf;
    long sent = 0;
    while (sent < total)
    {
        ssize_t sret = send(sockfd, p + sent, total - sent, MSG_NOSIGNAL);
        if (sret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            return -1;
        }
        sent += (long)sret;
    }
    return sent;
}

// 接收认证信息并校验密码
int UserAuthen(int sockfd)
{
    train_t train;

    // 用户名接收
    char username[64] = {0};
    memset(&train, 0, sizeof(train));
    recvn(sockfd, &train.length_, sizeof(int));
    recvn(sockfd, train.data_, train.length_);
    memcpy(username, train.data_, train.length_);
    username[train.length_] = '\0';

    // 加密密码接收
    char cryptPassword[128] = {0};
    memset(&train, 0, sizeof(train));
    recvn(sockfd, &train.length_, sizeof(int));
    recvn(sockfd, train.data_, train.length_);
    memcpy(cryptPassword, train.data_, train.length_);
    cryptPassword[train.length_] = '\0';

    // 用户密码对比：0表示认证成功，-1表示认证失败，1表示查询用户信息失败
    int auth_ret = authenticateUser(username, cryptPassword);
    if (auth_ret == AUTH_RESULT_OK)
    {
        const char *success_msg = "AUTH_RESULT_OK";
        memset(&train, 0, sizeof(int));
        train.length_ = strlen(success_msg);
        memcpy(train.data_, success_msg, train.length_);
        if (send(sockfd, &train, sizeof(int)+train.length_, MSG_NOSIGNAL) <= 0)
        {
            return AUTH_RESULT_IO_ERROR;
        }
        return AUTH_RESULT_OK;
    }

    if (auth_ret == AUTH_RESULT_USER_QUERY_FAIL || auth_ret == AUTH_RESULT_FAIL)
    {
        const char *fail_msg = "AUTH_FAIL";
        if (send(sockfd, fail_msg, strlen(fail_msg), MSG_NOSIGNAL) < 0)
        {
            return AUTH_RESULT_IO_ERROR;
        }
        return auth_ret;
    }

    // 防御式兜底：未知认证结果按查询用户失败处理，保留重试语义
    return AUTH_RESULT_USER_QUERY_FAIL;
}
