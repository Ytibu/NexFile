#ifndef __CMDHANDLE_H__
#define __CMDHANDLE_H__

#include "../../shared/protocol.h"
#include "../../shared/logger.h"

int cmdParse(int sockFd, packetCmd_t *cmd);         // 解析命令并执行相应操作
int recvCmdSignal(int sockFd, cmdSignal_t *signal); // 接收并打印命令结果信号
int sendCmdSignal(int sockFd, cmdSignal_t *signal); // 发送命令结果信号

int changeDir(int sockFd, const char *path); // 切换目录
int listDir(int sockFd, const char *path);   // 列出目录内容

int printWorkingDir(int sockFd); // 打印当前工作目录

int PutFile(int sockFd, const char *path);    // 上传文件
int GetFile(int sockFd, const char *path);    // 下载文件
int removeFile(int sockFd, const char *path); // 删除文件或目录
int makeDir(int sockFd, const char *path);    // 创建目录

#endif