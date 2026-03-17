#include "../include/cmdHandle.h"

#include "../../shared/protocol.h"
#include "../include/sendMessage.h"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>

static char currentDir[256] = "/home/dingjr/DevCode/NexFile/tmp/netDisk/";

// 解析命令并执行相应操作
int cmdParse(int sockFd, packetCmd_t *cmd)
{
    printf("will Parser\n");
    switch (cmd->cmdCode_)
    {
    case REQ_CD:
        changeDir(sockFd, NULL);
        break;
    case REQ_LS:
        listDir(sockFd, cmd->data_);
        break;
    case REQ_MKDIR:
        makeDir(sockFd, cmd->data_);
        break;
    case REQ_RM:
        removeFile(sockFd, cmd->data_);
        break;
    case REQ_PUT:
        PutFile(sockFd, cmd->data_);
        break;
    case REQ_GET:
        GetFile(sockFd, cmd->data_);
        break;
    case REQ_PWD:
        printWorkingDir(sockFd);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

// 接收命令结果信号，并打印服务端/客户端之间传递的状态信息
int recvCmdSignal(int sockFd, cmdSignal_t *signal)
{
    int headerSize = sizeof(cmdSignal_t);
    if (signal != NULL)
    {
        return -1;
    }

    if (recvn(sockFd, signal, headerSize) <= 0)
    {
        LOG_ERROR("recvn header failed\n");
        return -1;
    }

    if (signal->length_ > 0)
    {
        char *msgBuf = (char *)calloc(1, signal->length_ + 1);
        if (msgBuf)
            return -1;

        if (recvn(sockFd, msgBuf, signal->length_) <= 0)
        {
            free(msgBuf);
            LOG_ERROR("recvn message failed\n");
            return -1;
        }

        // 仅用于调试打印，业务参数通过 cmdArg_.data_ 传递。
        printf("request message: %s\n", msgBuf);
        free(msgBuf);
    }

    printf("===== Cmd Result =====\n");
    printf("cmdCode: %u\n", signal->cmdArg_.cmdCode_);
    printf("status : %d\n", signal->cmdStatus_);
    printf("recvACK: %d\n", signal->recvACK_);

    printf("======================\n");

    return 0;
}

// 发送命令结果信号，支持携带可选 message_ 数据
int sendCmdSignal(int sockFd, cmdSignal_t *signal)
{
    size_t totalLen;
    if (signal == NULL)
    {
        return -1;
    }

    signal->recvACK_ = 1;
    totalLen = sizeof(cmdSignal_t) + (size_t)signal->length_;

    if (sendn(sockFd, signal, (long)totalLen) <= 0)
    {
        perror("sendn cmdSignal failed");
        return -1;
    }

    return 0;
}
// 切换目录
int changeDir(int sockFd, const char *path)
{
    cmdSignal_t reqSignal;
    cmdSignal_t *respSignal;
    const char *message;
    CmdStatus status;
    size_t msgLen;
    size_t totalLen;
    const char *targetPath = path;

    memset(&reqSignal, 0, sizeof(reqSignal));
    if (recvCmdSignal(sockFd, &reqSignal) != 0)
    {
        return -1;
    }

    if (reqSignal.cmdArg_.data_[0] != '\0')
    {
        targetPath = reqSignal.cmdArg_.data_;
    }

    if (targetPath == NULL || targetPath[0] == '\0')
    {
        targetPath = ".";
    }

    if (chdir(targetPath) == -1)
    {
        perror("chdir");
        status = CMD_FAILURE;
        message = "change directory failed";
    }
    else
    {
        status = CMD_SUCCESS;
        message = "change directory success";
    }

    msgLen = strlen(message);
    totalLen = sizeof(cmdSignal_t) + msgLen;
    respSignal = (cmdSignal_t *)calloc(1, totalLen);
    if (respSignal == NULL)
    {
        return -1;
    }

    memcpy(&respSignal->cmdArg_, &reqSignal.cmdArg_, sizeof(packetCmd_t));
    respSignal->sendACK_ = reqSignal.sendACK_;
    respSignal->cmdStatus_ = status;
    respSignal->length_ = (int)msgLen;
    memcpy(respSignal->message_, message, msgLen);

    if (sendCmdSignal(sockFd, respSignal) != 0)
    {
        free(respSignal);
        return -1;
    }

    free(respSignal);

    return status == CMD_SUCCESS ? 0 : -1;
}

// 列出目录内容
int listDir(int sockFd, const char *path)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        { // 仅过滤 . 和 ..，其余（包含隐藏文件和目录）都显示
            printf("%s  ", entry->d_name);
        }
    }
    printf("\n");

    closedir(dir);
    return 0;
}

// 打印当前工作目录
int printWorkingDir(int sockFd)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current working directory: %s\n", cwd);
    }
    else
    {
        perror("getcwd() error");
        return -1;
    }

    return 0;
}

// 上传文件 -- 服务器接收文件
int PutFile(int sockFd, const char *path)
{
    train_t train;
    memset(&train, 0, sizeof(train));

    // 接收文件名
    char filename[256];
    recvn(sockFd, &train.length_, sizeof(int));
    recvn(sockFd, train.data_, train.length_);
    memcpy(filename, train.data_, train.length_);
    printf("Received filename: %s\n", filename);

    off_t filesize;
    recvn(sockFd, &train.length_, sizeof(int));
    recvn(sockFd, train.data_, train.length_);
    memcpy(&filesize, train.data_, train.length_);
    printf("Received filesize: %ld\n", filesize);

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0664);
    ERROR_CHECK(fd, -1, "open");
    ftruncate(fd, filesize);
    char *pFd = (char *)mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ERROR_CHECK(pFd, MAP_FAILED, "mmap");

    recvn(sockFd, pFd, filesize);

    close(fd);
    return 0;
}

// 下载文件 -- 服务器发送文件
int GetFile(int sockFd, const char *path)
{
    train_t train;
    memset(&train, 0, sizeof(train));

    // 文件名发送
    train.length_ = strlen(path);
    memcpy(train.data_, path, train.length_ + 1);
    int send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if (send_ret < 0)
    {
        LOG_ERROR("Failed to send file name to client\n");
        return -1;
    }

    int fd = open(path, O_RDWR);
    ERROR_CHECK(fd, -1, "open");

    // 文件大小发送
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length_ = sizeof(off_t);
    memcpy(train.data_, &statbuf.st_size, train.length_);
    send_ret = send(sockFd, &train, sizeof(train.length_) + train.length_, MSG_NOSIGNAL);
    if (send_ret < 0)
    {
        LOG_ERROR("Failed to send file size to client\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

// 删除文件或目录
int removeFile(int sockFd, const char *path)
{
    int result = unlink(path);
    if (result == -1)
    {
        perror("remove");
        return -1;
    }
    return 0;
}

// 创建目录
int makeDir(int sockFd, const char *path)
{
    int result = mkdir(path, 0755);
    if (result == -1)
    {
        perror("mkdir");
        return -1;
    }
    return result;
}