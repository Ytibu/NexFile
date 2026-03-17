#include "../include/cmdOpt.h"

#include "../include/clientsendMessage.h"
#include "../../shared/protocol.h"
#include "../../shared/logger.h"
#include "../include/fileOpt.h"

#include <stdio.h>
#include <sys/socket.h>

// 切割命令：预防内容为空或内存越界，返回-1表示命令不合法，0表示合法
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
    if (token != NULL)
    {
        size_t len = strlen(token);
        if (len >= sizeof(pcmdArg->data_))
        {
            pcmdArg->cmdCode_ = REQ_INVALID;
            LOG_ERROR("Argument too long for command code %d: %s\n", pcmdArg->cmdCode_, token);
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

// 验证命令合法性：该有参数的必须带参，不该的禁止带参，返回-1表示不合法，0表示合法
int cmdVeri(packetCmd_t *pcmdArg)
{
    if (pcmdArg->cmdCode_ == REQ_INVALID)
    {
        return -1; // 命令不合法
    }

    // 1-2参数可选，4-7必须带参数，pwd不能带参数
    if(pcmdArg->argFlag_ >= 4 && pcmdArg->argFlag_ <= 7) // 这些命令必须带参数
    {
        if (pcmdArg->argFlag_ != 1 || pcmdArg->length_ <= 0 || strlen(pcmdArg->data_) == 0)
        {
            return -1; // 参数标志与长度不匹配
        }
    }else if(pcmdArg->cmdCode_ == REQ_PWD) // pwd命令不能带参数
    {
        if (pcmdArg->argFlag_ != 0 || pcmdArg->length_ != 0 || strlen(pcmdArg->data_) != 0)
        {
            return -1; // pwd命令不应带参数
        }
    }

    return 0; // 命令合法
}

// 先切分，再验证，返回0表示合法，-1表示不合法
int cmdCheck(char *cmd, packetCmd_t *pcmdArg)
{
    int ret = cmdCut(cmd, pcmdArg);
    if (ret != 0)
    {
        return ret; // 切割命令失败，命令不合法
    }
    ret = cmdVeri(pcmdArg);
    return ret; // 返回0表示命令合法，非0表示命令不合法
}