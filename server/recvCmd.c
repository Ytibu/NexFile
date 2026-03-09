#include "../include/server/recvCmd.h"

#include <errno.h>
#include <sys/socket.h>

// 解决半包问题，要一次性读取完所有数据
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

int recvCmd(int sockfd, PacketCmd_t *header)
{
    ssize_t n = recvn(sockfd, &header->cmdCode_, sizeof(uint32_t));
    if (n <= 0)
    {
        return (int)n;
    }

    n = recvn(sockfd, &header->length_, sizeof(uint32_t));
    if (n <= 0)
    {
        return (int)n;
    }

    if (header->length_ > sizeof(header->data_))
    {
        errno = EMSGSIZE;
        return -1;
    }

    // 空负载是合法场景：例如仅有命令码、没有参数（如 "cd"）。
    if (header->length_ == 0)
    {
        header->data_[0] = '\0';
        return (int)(sizeof(uint32_t) * 2);
    }

    n = recvn(sockfd, header->data_, header->length_);
    if (n <= 0)
    {
        return (int)n;
    }

    return (int)(sizeof(uint32_t) * 2 + header->length_);
}