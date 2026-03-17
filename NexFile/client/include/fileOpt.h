#ifndef __FILE_OPT_H__
#define __FILE_OPT_H__

#include "../../shared/protocol.h"

int cmdParse(int sockFd, packetCmd_t *cmd);        // 解析命令并执行相应操作
int sendCmdSignal(int sockFd, packetCmd_t *cmd); // 传参cmd，函数内部初始化cmdSignal结构体并发送命令包到服务器
int recvCmdSignal(int sockFd); // 接收服务器返回的命令执行结果，并处理ACK等信息

int changeDir(int sockFd, packetCmd_t *cmd);       // 切换目录
int listDir(int sockFd, packetCmd_t *cmd);         // 列出目录内容
int printWorkingDir(int sockFd, packetCmd_t *cmd); // 打印当前工作目录
int removeFile(int sockFd, packetCmd_t *cmd);      // 删除文件或目录
int putFile(int sockFd, packetCmd_t *cmd);         // 上传文件
int getFile(int sockFd, packetCmd_t *cmd);         // 下载文件
int makeDir(int sockFd, packetCmd_t *cmd);         // 创建目录

#endif // __FILE_OPT_H__