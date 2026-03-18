#include "../include/fileOpt.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "../../shared/protocol.h"
#include "../../shared/logger.h"
#include "../include/clientsendMessage.h"
#include "../include/clientConfig.h"

// 解析命令并执行相应操作
int cmdParse(int sockFd, packetCmd_t *cmd)
{
    sendCmd(sockFd, cmd);   // 发送命令包到服务器
    switch (cmd->cmdCode_)
    {
    case REQ_CD:
        changeDir(sockFd, cmd);
        break;
    case REQ_LS:
        listDir(sockFd, cmd);
        break;
    case REQ_MKDIR:
        makeDir(sockFd, cmd);
        break;
    case REQ_RM:
        removeFile(sockFd, cmd);
        break;
    case REQ_PUT:
        putFile(sockFd, cmd);
        break;
    case REQ_GET:
        getFile(sockFd, cmd);
        break;
    case REQ_PWD:
        printWorkingDir(sockFd, cmd);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

// 处理命令前的准备工作，如参数验证等
int sendCmd(int sockFd, packetCmd_t *cmd)
{
    if (cmd == NULL)
    {
        LOG_ERROR("Command argument is NULL\n");
        return -1;
    }

    int send_ret = send(sockFd, cmd, sizeof(packetCmd_t), MSG_NOSIGNAL);
    if(send_ret < 0)
    {
        LOG_ERROR("Failed to send command signal to server\n");
        return -1;
    }
    return 0;
}

// 处理服务器返回的命令执行结果，如解析ACK等
int recvSignal(int sockFd)
{
    cmdSignal_t cmdSignal;
    memset(&cmdSignal, 0, sizeof(cmdSignal));
    int recv_ret = recvn(sockFd, &cmdSignal, sizeof(cmdSignal));
    if (recv_ret < 0)    {
        LOG_ERROR("Failed to receive command signal from server\n");
        return -1;
    }
    if (recv_ret == 0)    {
        LOG_ERROR("Server closed the connection while waiting for command signal\n");
        return -1;
    }

    if (cmdSignal.length_ < 0 || cmdSignal.length_ > 1024)
    {
        LOG_ERROR("Invalid command signal message length\n");
        return -1;
    }

    char *msgBuf = NULL;
    if (cmdSignal.length_ > 0)
    {
        msgBuf = (char *)calloc((size_t)cmdSignal.length_ + 1, 1);
        if (msgBuf == NULL)
        {
            LOG_ERROR("Failed to allocate memory for command signal message\n");
            return -1;
        }

        recv_ret = recvn(sockFd, msgBuf, cmdSignal.length_);
        if (recv_ret <= 0)
        {
            LOG_ERROR("Failed to receive command signal message\n");
            free(msgBuf);
            return -1;
        }
    }

    printf("Command execution result: %s\n", msgBuf == NULL ? "" : msgBuf);
    free(msgBuf);

    return 0;
}

// 切换目录
int changeDir(int sockFd, packetCmd_t *cmd)
{
    // TODO
    recvSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

// 列出目录内容
int listDir(int sockFd, packetCmd_t *cmd) // 列出目录内容
{
    // TODO
    recvSignal(sockFd); // 处理服务器返回的命令执行结果

    return 0;
}

// 打印当前工作目录
int printWorkingDir(int sockFd, packetCmd_t *cmd) // 打印当前工作目录
{
    if(cmd->argFlag_ == 1)
    {
        LOG_ERROR("pwd command does not take any arguments\n");
        return -1;
    }
    // TODO
    recvSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

// 删除文件或目录
int removeFile(int sockFd, packetCmd_t *cmd) // 删除文件或目录
{
    if (cmd->argFlag_ != 1)
    {
        LOG_ERROR("rm command requires an argument (file or directory path)\n");
        return -1;
    }

    // TODO
    recvSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

// 文件从本地发送到服务器
int putFile(int sockFd, packetCmd_t *cmd)
{
    if (cmd->argFlag_ != 1)
    {
        LOG_ERROR("put command requires an argument (file path)\n");
        return -1;
    }

    char filePath[1024];
    snprintf(filePath, sizeof(filePath), "%s/%s", CLIENT_FILE_PATH, cmd->data_);

    train_t train;
    train.length_ = strlen(cmd->data_);
    memcpy(train.data_, cmd->data_, train.length_ + 1);
    int send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if (send_ret < 0)
    {
        LOG_ERROR("Failed to send file name to server\n");
        return -1;
    }

    int fd = open(cmd->cmdCode_, O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    // 文件大小发送
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length_ = sizeof(off_t);
    memcpy(train.data_, &statbuf.st_size, train.length_);
    send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if (send_ret < 0)
    {
        LOG_ERROR("Failed to send file size to server\n");
        close(fd);
        return -1;
    }

    recvSignal(sockFd); // 处理服务器返回的命令执行结果

    close(fd);
    return 0;
}

// 本地等待服务器发送文件并保存到本地
int getFile(int sockFd, packetCmd_t *cmd)
{
    if (cmd->argFlag_ != 1)
    {
        LOG_ERROR("get command requires an argument (file path)\n");
        return -1;
    }

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

    // 创建本地文件并映射内存
    int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0666);
    ERROR_CHECK(fd, -1, "open");
    ftruncate(fd, filesize);
    char *pFd = (char *)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(pFd, MAP_FAILED, "mmap");

    recvn(sockFd, pFd, filesize);

    close(fd);
    recvSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

int makeDir(int sockFd, packetCmd_t *cmd)
{
    if (cmd->argFlag_ != 1)
    {
        LOG_ERROR("mkdir command requires an argument (directory path)\n");
        return -1;
    }
    // TODO
    recvSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}