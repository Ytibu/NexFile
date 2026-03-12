#ifndef __RECVCMD_H__
#define __RECVCMD_H__
#include "../../shared/protocol.h"
#include "../../shared/logger.h"

//int recvn(int sockFd, void *buf, long total);
int recvCmd(int sockfd, packetCmd_t *header);

#endif // __RECVCMD_H__