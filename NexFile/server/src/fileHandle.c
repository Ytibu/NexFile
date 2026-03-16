#include "../include/fileHandle.h"

#include "../shared/protocol.h"
#include "../shared/logger.h"

#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

int recvFile(int sockFd)
{
    // 接收文件名
    char fileName[128] = {0};
    train_t train;
    recvn(sockFd, &train.length_, sizeof(int));
    recvn(sockFd, train.data_, train.length_);
    memcpy(fileName, train.data_, train.length_);

    // 接收文件大小
    off_t filesize;
    recvn(sockFd, &train.length_, sizeof(int));
    recvn(sockFd, &train.data_, train.length_);
    memcpy(&filesize, train.data_, train.length_);

    // 创建文件
    int fd = open(fileName, O_RDWR | O_CREAT, 0664);
    ERROR_CHECK(fd, -1, "open");

    ftruncate(fd, filesize);
    char *fileAddr = (char*)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(fileAddr, MAP_FAILED, "mmap");
    recvn(sockFd, fileAddr, filesize);

    close(fd);
    return 0;
}

int sendFile(int sockFd, const char* fileName)
{
    train_t train;
    
    // 发送文件名
    train.length_ = strlen(fileName);
    memcpy(train.data_, fileName, train.length_);
    send(sockFd, &train, sizeof(int) + train.length_, MSG_NOSIGNAL);
    
    // 发送文件大小
    struct stat fStat;
    int fd = open(fileName, O_RDONLY);
    ERROR_CHECK(fd, -1, "open");
    fstat(fd, &fStat);

    train.length_ = sizeof(off_t);
    memcpy(train.data_, &fStat.st_size, train.length_);
    send(sockFd, &train, sizeof(int) + train.length_, MSG_NOSIGNAL);

    // 发送文件内容
    sendfile(sockFd, fd, NULL, fStat.st_size);

    close(fd);
    return 0;
}