#ifndef __RECVCMD_H__
#define __RECVCMD_H__
#include "../protocol.h"
#include "../logger.h"

int recvn(int sockFd, void *buf, long total);
int recvCmd(int sockfd, PacketCmd_t *header);

#endif // __RECVCMD_H__