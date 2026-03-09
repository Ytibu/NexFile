// protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <string.h>

// 1. 定义命令类型枚举（服务器和客户端共用）
typedef enum
{
    REQ_INVALID = 0,
    REQ_CD = 1,
    REQ_LS,
    REQ_MKDIR,
    REQ_RM,
    REQ_PUT,
    REQ_GET,
    REQ_PWD,
    // ... 其他命令
} CommandCode;

// 2. 定义网络传输的包结构
typedef struct PacketCmd_s
{
    uint32_t cmdCode_; // 命令代码（上面的枚举值）
    uint32_t length_;  // 后续数据的长度
    char data_[1024];      // 紧接着是可变长度的数据（如路径名）
} PacketCmd_t;

// 3. 辅助函数：将字符串命令转换为枚举
static inline CommandCode str_to_cmdcode(const char *cmd_str)
{
    if (cmd_str == NULL)
        return REQ_INVALID;

    if (strcmp(cmd_str, "cd") == 0)
        return REQ_CD;
    if (strcmp(cmd_str, "ls") == 0)
        return REQ_LS;
    if (strcmp(cmd_str, "mkdir") == 0)
        return REQ_MKDIR;
    if (strcmp(cmd_str, "rm") == 0)
        return REQ_RM;
    if (strcmp(cmd_str, "put") == 0)
        return REQ_PUT;
    if (strcmp(cmd_str, "get") == 0)
        return REQ_GET;
    if (strcmp(cmd_str, "pwd") == 0)
        return REQ_PWD;
    return REQ_INVALID;
}

#endif