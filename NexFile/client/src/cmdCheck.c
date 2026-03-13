#include "../include/cmdCheck.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmdCut(char *cmd, packetCmd_t *pcmdArg)
{
    char *token;
    char *saveptr = NULL;

    if (cmd == NULL || pcmdArg == NULL)
    {
        return -1;
    }

    // 初始化输出结构
    pcmdArg->cmdCode_ = REQ_INVALID;
    pcmdArg->argFlag_ = 0;
    pcmdArg->length_ = 0;
    pcmdArg->data_[0] = '\0';

    // 获取第一个标记
    token = strtok_r(cmd, " \t\r\n", &saveptr);
    if (token == NULL)
    {
        return -1;
    }

    pcmdArg->cmdCode_ = str_to_cmdcode(token);
    token = strtok_r(NULL, " \t\r\n", &saveptr);
    if (token != NULL) {
        size_t len = strlen(token);
        if(len >= sizeof(pcmdArg->data_)) {
            pcmdArg->cmdCode_ = REQ_INVALID;
            return -1; // 参数过长
        }

        memcpy(pcmdArg->data_, token, len + 1);
        pcmdArg->argFlag_ = 1;
        pcmdArg->length_ = (int)len;

        // 若还有更多参数，判定为非法（可按需求改成支持多参数）
        if (strtok_r(NULL, " \t\r\n", &saveptr) != NULL)
        {
            pcmdArg->cmdCode_ = REQ_INVALID;
            pcmdArg->argFlag_ = 0;
            pcmdArg->length_ = 0;
            pcmdArg->data_[0] = '\0';
            return -1;
        }
    }


    return 0;
}

int cmdVeri(packetCmd_t *pcmdArg)
{
    if (pcmdArg->cmdCode_ == REQ_INVALID)
    {
        return -1; // 命令不合法
    }
    return 0; // 命令合法
}

int cmdCheck(char *cmd, packetCmd_t *pcmdArg)
{
    if(cmdCut(cmd, pcmdArg) != 0)
    {
        return -1; // 切割命令失败，命令不合法
    }
    return cmdVeri(pcmdArg);
}