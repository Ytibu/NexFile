#include "../include/sendCmd.h"
#include "../../shared/logger.h"

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>


int sendCmd(int sockfd, PacketCmd_t *pcmdArg)
{
    ssize_t n = send(sockfd, &pcmdArg->cmdCode_, sizeof(uint32_t), 0);
    ERROR_CHECK(n, -1, "send cmdCode");
    n = send(sockfd, &pcmdArg->length_, sizeof(uint32_t), 0);
    ERROR_CHECK(n, -1, "send length");
    n = send(sockfd, pcmdArg->data_, pcmdArg->length_, 0);
    ERROR_CHECK(n, -1, "send data");
    
    return 0;
}  