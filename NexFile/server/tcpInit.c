#include "tcpInit.h"
#include  "../shared/logger.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>


int tcpInit(char *ip, int port, int *sockFd) {
    *sockFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(*sockFd, -1, "socket");

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    int reuse = 1;
    if (setsockopt(*sockFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        perror("setsockopt");
        close(*sockFd);
        return -1;
    }

    if (bind(*sockFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(*sockFd);
        return -1;
    }

    if (listen(*sockFd, 5) == -1) {
        perror("listen");
        close(*sockFd);
        return -1;
    }

    return 0;
}