#include "../include/clientsendMessage.h"
#include "../../shared/protocol.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

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

int sendPassword(int fd, const char *username, const char *password)
{
    if (username == NULL || password == NULL)
    {
        return -1;
    }

    size_t usernameLen = strlen(username);
    size_t passwordLen = strlen(password);
    if (usernameLen > sizeof(((train_t *)0)->data_) || passwordLen > sizeof(((train_t *)0)->data_))
    {
        return -1;
    }

    train_t train;
    memset(&train, 0, sizeof(train));
    train.length_ = (int)usernameLen;
    memcpy(train.data_, username, usernameLen);
    if (sendn(fd, &train, sizeof(train.length_) + (size_t)train.length_) < 0)
    {
        return -1;
    }

    memset(&train, 0, sizeof(train));
    train.length_ = (int)passwordLen;
    memcpy(train.data_, password, passwordLen);
    if (sendn(fd, &train, sizeof(train.length_) + (size_t)train.length_) < 0)
    {
        return -1;
    }
    return 0;
}