#include "../include/fileOpt.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../include/config.h"
#include "../shared/protocol.h"
#include "../include/clientsendMessage.h"


int putFile(const char *fileName, int sockFd)
{
    // 按结构体中的定长数组保存路径
    const Config_t *g_clientConfig = GetGlobalConfig();
    char filePath[1024];
    snprintf(filePath, sizeof(filePath), "%s%s", g_clientConfig->filePath, fileName);

    train_t train;
    train.length_ = strlen(fileName);
    memcpy(train.data_, fileName, train.length_ + 1);
    int send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if (send_ret < 0)
    {
        LOG_ERROR("Failed to send file name to server\n");
        return -1;
    }

    int fd = open(fileName, O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    // 文件大小发送
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length_ = sizeof(off_t);
    memcpy(train.data_, &statbuf.st_size, train.length_);
    send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if(send_ret < 0)
    {
        LOG_ERROR("Failed to send file size to server\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int getFile(int sockFd)
{
    // 文件名发送
    char fileName[1024] = {0};
    train_t train;
    recvn(sockFd, &train.length_, sizeof(train.length_));
    recvn(sockFd, train.data_, train.length_);
    memcpy(fileName, train.data_, train.length_);

    // 文件长度接收
    off_t filesize;
    recvn(sockFd, &train.length_, sizeof(train.length_));
    recvn(sockFd, &train.data_, train.length_);
    memcpy(&filesize, train.data_, train.length_);
    printf("filesize = %ld\n", filesize);

    int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(fd, -1, "open");

    ftruncate(fd, filesize);
    char *pFd = (char *)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(pFd, MAP_FAILED, "mmap");

    recvn(sockFd, pFd, filesize);

    close(fd);

    return 0;
}