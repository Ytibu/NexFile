#ifndef __SENDCMD_H__
#define __SENDCMD_H__

#include "../../shared/protocol.h"

int sendCmd(int sockfd, PacketCmd_t *pcmdArg);    // 发送 PacketHeader 结构体到服务器

#endif // __SENDCMD_H__