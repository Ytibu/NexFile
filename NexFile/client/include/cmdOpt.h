#ifndef __CMDOPT_H__
#define __CMDOPT_H__
#include "../../shared/protocol.h"

// 切割命令，返回切割后的参数数量
int cmdCut(char *cmd, packetCmd_t *pcmdArg);

// 验证命令是否合法，返回1合法，0不合法
int cmdVeri(packetCmd_t *pcmdArg); 

// 综合切割和验证命令，返回1合法，0不合法
int cmdCheck(char *cmd, packetCmd_t *pcmdArg); 

// 发送命令到服务器，返回0成功，-1失败
int sendCmd(int sockfd, packetCmd_t *pcmdArg);

// 命令处理器
int handleCommand(int sockfd, packetCmd_t *pcmdArg);

#endif // __CMDOPT_H__