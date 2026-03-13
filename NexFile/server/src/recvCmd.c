#include "../include/recvCmd.h"
#include "../include/sendMessage.h"

#include <errno.h>
#include <sys/socket.h>

int recvCmd(int sockfd, packetCmd_t *header)
{
    // 接收整个结构
    int total_len = sizeof(packetCmd_t);
    int received = recv(sockfd, header, total_len, MSG_WAITALL);
    printf("recvCmd: received %d bytes, expected %d bytes\n", received, total_len);

    if (received != total_len)
    {
        // 处理接收错误
        return -1;
    }

    // 可以根据实际数据长度进行验证
    if (header->length_ > 1024)
    {
        // 数据长度不合法
        return -2;
    }

    return received;
}