#include "../include/sendCmd.h"
#include "../../shared/logger.h"

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>


int sendCmd(int sockfd, packetCmd_t *pcmdArg)
{
    int total_len = sizeof(packetCmd_t);
    int sent = send(sockfd, pcmdArg, total_len, 0);
    if (sent != total_len) {
        perror("sendCmd");
        return -1;
    }
    return 0;
}