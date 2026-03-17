#include "../include/fileOpt.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../../shared/protocol.h"
#include "../../shared/logger.h"
#include "../include/clientsendMessage.h"
#include "../include/clientConfig.h"

// 解析命令并执行相应操作
int cmdParse(int sockFd, packetCmd_t *cmd)
{
    switch (cmd->cmdCode_)
    {
    case REQ_CD:
        changeDir(sockFd, &cmd);
        break;
    case REQ_LS:
        listDir(sockFd, &cmd);
        break;
    case REQ_MKDIR:
        makeDir(sockFd, &cmd);
        break;
    case REQ_RM:
        removeFile(sockFd, &cmd);
        break;
    case REQ_PUT:
        putFile(sockFd, &cmd);
        break;
    case REQ_GET:
        getFile(sockFd, &cmd);
        break;
    case REQ_PWD:
        printWorkingDir(sockFd, &cmd);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

// 处理命令前的准备工作，如参数验证等
int sendCmdSignal(int sockFd, packetCmd_t *cmd)
{
    if (cmd == NULL)
        return -1;
    // 利用cmd结构体初始化命令信号结构体,然后发送给服务器
    int msgLen = 0;
    size_t totalLen = sizeof(cmdSignal_t) + msgLen;

    cmdSignal_t *signal = (cmdSignal_t *)calloc(1, totalLen);
    if (!signal)
        return -1;

    memset(signal, 0, totalLen);
    memcpy(&signal->cmdArg_, cmd, sizeof(packetCmd_t));
    signal->sendACK_ = 1;              // 标记客户端已发送
    signal->recvACK_ = 0;              // 等待服务器响应
    signal->cmdStatus_ = CMD_NOHANDLE; // 初始状态为未处理
    signal->length_ = msgLen;          // message_ 的实际长度
    
    // 注意：在实际网络编程中，建议使用循环 send 确保数据完全发出
    sendn(sockFd, signal, (long)totalLen); // 发送命令信号结构体到服务器

    // 5. 清理内存
    free(signal);
    return 0;
}

// 处理服务器返回的命令执行结果，如解析ACK等
int recvCmdSignal(int sockFd)
{
    // 1. 先接收固定头（不包括 message_）
    int headerSize = sizeof(cmdSignal_t);
    cmdSignal_t header;

    if (recvn(sockFd, &header, headerSize) <= 0)
    {
        perror("recvn header failed");
        return -1;
    }

    // 2. 如果有 message，再接收
    char *msgBuf = NULL;
    if (header.length_ > 0)
    {
        msgBuf = (char *)calloc(1, header.length_ + 1);
        if (!msgBuf) return -1;

        if (recvn(sockFd, msgBuf, header.length_) <= 0)
        {
            perror("recvn message failed");
            free(msgBuf);
            return -1;
        }
    }

    // 3. 解析并打印
    printf("===== Cmd Result =====\n");
    printf("cmdCode: %u\n", header.cmdArg_.cmdCode_);
    printf("status : %d\n", header.cmdStatus_);
    printf("recvACK: %d\n", header.recvACK_);

    if (msgBuf)
    {
        printf("message: %s\n", msgBuf);
        free(msgBuf);
    }

    printf("======================\n");

    return 0;
}

// 切换目录
int changeDir(int sockFd, packetCmd_t *cmd)
{
    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体
    // TODO
    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

// 列出目录内容
int listDir(int sockFd, packetCmd_t *cmd) // 列出目录内容
{
    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体
    // TODO
    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果

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
    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体
    // TODO
    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果
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

    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体
    // TODO
    recvCmdSignal(cmd); // 处理服务器返回的命令执行结果
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

    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体

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

    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果

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

    cmdSignal_t cmdSignal;
    sendCmdSignal(cmd, &cmdSignal);                            // 初始化命令信号结构体
    send(sockFd, &cmdSignal, sizeof(cmdSignal), MSG_NOSIGNAL); // 发送命令包

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
    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}

int makeDir(int sockFd, packetCmd_t *cmd)
{
    if (cmd->argFlag_ != 1)
    {
        LOG_ERROR("mkdir command requires an argument (directory path)\n");
        return -1;
    }
    cmdSignal_t cmdSignal;
    sendCmdSignal(sockFd, cmd); // 初始化命令信号结构体
    // TODO
    recvCmdSignal(sockFd); // 处理服务器返回的命令执行结果
    return 0;
}