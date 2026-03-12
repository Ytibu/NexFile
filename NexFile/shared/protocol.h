#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <string.h>

// 定义网络传输的包结构
typedef struct train_s
{
    int length_; // 后续数据的长度
    char data_[1024];      // 紧接着是可变长度的数据（如路径名）
} train_t;

// 2. 定义网络传输命令的包结构
typedef struct packetCmd_s
{
    uint32_t cmdCode_;  // 命令代码（上面的枚举值）
    uint32_t argFlag_;  // 是否带参数（0或1）
    int length_;        // 后续数据的长度
    char data_[1024];   // 参数内容
} packetCmd_t;

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
    REQ_PWD
} CommandCode;
// 0代表未知命令，1-7分别对应上面定义的命令类型


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