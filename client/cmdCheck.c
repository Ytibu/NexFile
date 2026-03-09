#include "../include/client/cmdCheck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// int cmdCut(char *cmd, PacketCmd_t *p);  // 切割命令，返回切割后的参数数量
// int cmdVeri(PacketCmd_t *pcmdArg); // 验证命令是否合法，返回1合法，0不合法

// int cmdCheck(char *cmd, PacketCmd_t *pcmdArg); // 综合切割和验证命令，返回1合法，0不合法

int cmdCut(char *cmd, PacketCmd_t *pcmdArg)
{
    char *token;
    char *saveptr = NULL;

    if (cmd == NULL || pcmdArg == NULL)
    {
        return -1;
    }

    strcpy(pcmdArg->data_, ""); // 初始化参数为空字符串
    pcmdArg->length_ = 0;

    // 获取第一个标记
    token = strtok_r(cmd, " \t\r\n", &saveptr);
    if (token == NULL)
    {
        pcmdArg->cmdCode_ = REQ_INVALID;
        return -1;
    }
    pcmdArg->cmdCode_ = str_to_cmdcode(token);

    // 继续获取参数，只有拿到有效 token 时才计数
    while ((token = strtok_r(NULL, " \t\r\n", &saveptr)) != NULL) {
        strcpy(pcmdArg->data_, token); // 这里假设只有一个参数，如果有多个参数需要修改结构体和逻辑
        pcmdArg->length_ = strlen(token);
    }

    //printf("cmdArg: cmd=%d, arg=%s\n", pcmdArg->cmdCode_, pcmdArg->data_ ? pcmdArg->data_ : "(null)");

    return 0;
}

int cmdVeri(PacketCmd_t *pcmdArg)
{
    if (pcmdArg->cmdCode_ != REQ_INVALID)
    {
        return 1; // 命令合法
    }
    return 0; // 命令不合法
}

int cmdCheck(char *cmd, PacketCmd_t *pcmdArg)
{
    cmdCut(cmd, pcmdArg);
    return cmdVeri(pcmdArg);
}