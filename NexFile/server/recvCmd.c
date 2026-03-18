#include "recvCmd.h"
#include "sendMessage.h"

#include <errno.h>
#include <sys/socket.h>

int recvCmd(int sockfd, packetCmd_t *header)
{
    // 接收整个结构
    ssize_t recv_ret = recvn(sockfd, header, sizeof(packetCmd_t));
    if (recv_ret < 0)    {
        perror("recv");
        return -1;
    }
    else if (recv_ret == 0)    {
        printf("Client closed the connection.\n");
        return 0;
    }

    return recv_ret;
}