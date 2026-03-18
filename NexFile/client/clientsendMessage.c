#include "clientsendMessage.h"
#include "../shared/protocol.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>

int recvn(int sockFd, void *buf, long total)
{
    char *p = (char *)buf;
    long cursize = 0;
    while (cursize < total)
    {
        ssize_t sret = recv(sockFd, p + cursize, total - cursize, 0);
        if (sret == 0)
        {
            return 1;
        }
        cursize += sret;
    }
    return 0;
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

int sendPassword(int fd, const char *username, const char *password)
{
    if (username == NULL || password == NULL)
    {
        return -1;
    }

    int userLen = strlen(username);
    int passLen = strlen(password);

    train_t train;

    memset(&train, 0, sizeof(train));
    train.length_ = userLen;
    memcpy(train.data_, username, userLen);
    if (sendn(fd, &train, sizeof(train.length_) + train.length_) < 0)
    {
        return -1;
    }

    memset(&train, 0, sizeof(train));
    train.length_ = passLen;
    memcpy(train.data_, password, passLen);
    if (sendn(fd, &train, sizeof(train.length_) + train.length_) < 0)
    {
        return -1;
    }
    return 0;
}