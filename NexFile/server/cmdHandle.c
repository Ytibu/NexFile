#include "cmdHandle.h"

#include  "../shared/protocol.h"
#include "sendMessage.h"
#include "clientHandle.h"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>


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
        listDir(sockFd, cmd);
        break;
    case REQ_MKDIR:
        makeDir(sockFd, cmd);
        break;
    case REQ_RM:
        removeFile(sockFd, cmd);
        break;
    case REQ_PUT:
        PutFile(sockFd, cmd);
        break;
    case REQ_GET:
        GetFile(sockFd, cmd);
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

// 发送命令结果信号，支持携带可选 message_ 数据
int sendSignal(int sockFd, char *message, CmdStatus status)
{
    cmdSignal_t cmdSignal;
    memset(&cmdSignal, 0, sizeof(cmdSignal));
    cmdSignal.sendACK_ = 1;
    cmdSignal.recvACK_ = 1;
    cmdSignal.cmdStatus_ = status; // 设置命令执行结果状态
    if (message != NULL)
    {
        size_t msgLen = strlen(message);
        if (msgLen > INT_MAX)
        {
            LOG_ERROR("sendSignal message too long\n");
            return -1;
        }
        cmdSignal.length_ = (int)msgLen;
    }

    int send_ret = sendn(sockFd, (char *)&cmdSignal, sizeof(cmdSignal));
    if (send_ret < 0)
    {
        perror("send cmdSignal header");
        return -1;
    }

    if (cmdSignal.length_ > 0)
    {
        send_ret = sendn(sockFd, message, cmdSignal.length_);
        if (send_ret < 0)
        {
            perror("send cmdSignal message");
            return -1;
        }
    }
    return 0;
}
// 切换目录
int changeDir(int sockFd, packetCmd_t *cmd)
{
    (void)sockFd;
    (void)cmd;
    return 0;
}

// 列出目录内容
int listDir(int sockFd, packetCmd_t *cmd)
{
    (void)sockFd;
    DIR *dir;
    struct dirent *entry;

    dir = opendir(cmd->data_);
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
int printWorkingDir(int sockFd, packetCmd_t *cmd)
{
    (void)sockFd;
    (void)cmd;
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
int PutFile(int sockFd, packetCmd_t *cmd)
{
    (void)cmd;
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
int GetFile(int sockFd, packetCmd_t *cmd)
{
    train_t train;
    memset(&train, 0, sizeof(train));
    char *path = cmd->data_;

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
int removeFile(int sockFd, packetCmd_t *cmd)
{
    (void)sockFd;
    int result = unlink(cmd->data_);
    if (result == -1)
    {
        perror("remove");
        return -1;
    }
    return 0;
}

// 创建目录
int makeDir(int sockFd, packetCmd_t *cmd)
{
    // 拼接clientPath和cmd->data_形成完整路径，并操作拼接后的内容，不修改clientPath的值
    char fullPath[MAX_PATH_LEN] = {0};
    int written = snprintf(fullPath, sizeof(fullPath), "%s/%s", clientPath, cmd->data_);
    if (written < 0 || written >= (int)sizeof(fullPath))
    {
        LOG_ERROR("Failed to construct full path for mkdir\n");
        return -1;
    }
    int result = mkdir(fullPath, 0755);
    printf("Attempting to create directory: %s\n", fullPath);
    if (result == -1)
    {
        char errorMsg[256] = "Failed to create directory";
        sendSignal(sockFd, errorMsg, CMD_FAILURE);
        perror("mkdir");
        return -1;
    }else{
        char successMsg[256] = "Directory created successfully";
        sendSignal(sockFd, successMsg, CMD_SUCCESS);
    }
    return result;
}