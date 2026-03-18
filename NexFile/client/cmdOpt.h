#ifndef __CMDOPT_H__
#define __CMDOPT_H__
#include  "../shared/protocol.h"

// 切割命令：预防内容为空或内存越界，返回-1表示命令不合法，0表示合法
int cmdCut(char *cmd, packetCmd_t *pcmdArg);

// 验证命令是否合法：该有参数的必须带参，不该的禁止带参，返回-1表示不合法，0表示合法
int cmdVeri(packetCmd_t *pcmdArg); 

// 先切分，再验证，返回0表示合法，-1表示不合法
int cmdCheck(char *cmd, packetCmd_t *pcmdArg); 

#endif // __CMDOPT_H__