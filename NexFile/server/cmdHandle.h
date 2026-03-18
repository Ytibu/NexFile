#ifndef __CMDHANDLE_H__
#define __CMDHANDLE_H__

#include  "../shared/protocol.h"
#include  "../shared/logger.h"

int cmdParse(int sockFd, packetCmd_t *cmd);         // 解析命令并执行相应操作
int sendSignal(int sockFd, char *message, CmdStatus status); // 发送命令结果信号，支持携带可选 message_ 数据

int changeDir(int sockFd, packetCmd_t *cmd); // 切换目录
int listDir(int sockFd, packetCmd_t *cmd);   // 列出目录内容

int printWorkingDir(int sockFd, packetCmd_t *cmd); // 打印当前工作目录

int PutFile(int sockFd, packetCmd_t *cmd);    // 上传文件
int GetFile(int sockFd, packetCmd_t *cmd);    // 下载文件
int removeFile(int sockFd, packetCmd_t *cmd); // 删除文件或目录
int makeDir(int sockFd, packetCmd_t *cmd);    // 创建目录

#endif