#ifndef __CMD_H__
#define __CMD_H__

#include <stdio.h>

typedef enum {
    CMD_CD,
    CMD_LS,
    CMD_PWD,
    CMD_PUTS,
    CMD_GETS,
    CMD_REMOVE,
    CMD_MKDIR,
    CMD_ERROR
} client_cmd_t;

typedef struct cmd_s{
    client_cmd_t cmd;
    char *args[3];
} cmd_t;

int send_all(int sockfd, const void *buf, size_t len);
int recv_all(int sockfd, void *buf, size_t len);

#endif