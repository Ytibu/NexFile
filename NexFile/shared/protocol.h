#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>
#include <string.h>

// 定义网络传输的包结构
typedef struct train_s
{
    int length_;  // 后续数据的长度
    char data_[1024]; // 紧接着是可变长度的数据（如路径名）
} train_t;

// 定义命令代码枚举
typedef enum
{
    REQ_INVALID = 0,
    REQ_CD = 1,
    REQ_LS = 2,
    REQ_PWD = 3,
    REQ_RM = 4,
    REQ_PUT = 5,
    REQ_GET = 6,
    REQ_MKDIR = 7
} CommandCode;

// 定义命令包结构
typedef struct packetCmd_s
{
    uint32_t cmdCode_; // 命令代码（上面的枚举值）
    uint32_t argFlag_; // 是否带参数（0或1）
    int length_;       // 后续数据的长度
    char data_[1024];      // 参数内容
} packetCmd_t;

// 定义命令执行结果状态
typedef enum
{
    CMD_SUCCESS = 1,    // 未处理
    CMD_NOHANDLE = 0,   // 未处理
    CMD_FAILURE = -1,   // 失败
    CMD_NOT_FOUND = -2, // 文件或目录未找到
    CMD_INVALID = -3    // 无效命令
} CmdStatus;

// 定义命令信号结构
typedef struct cmdSignal_s
{
    packetCmd_t cmdArg_;  // 命令代码
    int sendACK_;         // 客户端是否发送命令的ACK（0或1）
    int recvACK_;         // 服务器是否收到命令的ACK（0或1）
    CmdStatus cmdStatus_; // 命令执行结果状态
    int length_;          // 后续消息的长度
    char message_[];      // 服务器返回的消息（如错误信息或成功提示）
} cmdSignal_t;

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