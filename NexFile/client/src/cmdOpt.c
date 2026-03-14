#include "../include/cmdOpt.h"

#include "../include/clientsendMessage.h"
#include "../include/fileOpt.h"

#include <stdio.h>
#include <sys/socket.h>

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
    int ret = cmdCut(cmd, pcmdArg);
    if (ret != 0)
    {
        return ret; // 切割命令失败，命令不合法
    }
    ret = cmdVeri(pcmdArg);
    return ret; // 返回0表示命令合法，非0表示命令不合法
}

int sendCmd(int sockfd, packetCmd_t *pcmdArg)
{
    int total_len = sizeof(packetCmd_t);
    int sent = sendn(sockfd, pcmdArg, total_len);
    if (sent < 0)
    {
        perror("sendCmd");
        return -1;
    }
    return sent;
}

// 命令处理器
int handleCommand(int sockfd, packetCmd_t *pcmdArg)
{
    switch (pcmdArg->cmdCode_)
    {
    case REQ_CD:
        /* code */
        sendCmd(sockfd, pcmdArg);
        break;
    case REQ_LS:
        /* code */
        sendCmd(sockfd, pcmdArg);
        break;
    case REQ_MKDIR:
        /* code */
        sendCmd(sockfd, pcmdArg);
        break;
    case REQ_RM:
        /* code */
        sendCmd(sockfd, pcmdArg);
        break;
    case REQ_PUT:
        /* code */
        sendCmd(sockfd, pcmdArg);
        putFile(pcmdArg->data_, sockfd);
        break;
    case REQ_GET:
        /* code */
        sendCmd(sockfd, pcmdArg);
        getFile(sockfd);
        break;
    case REQ_PWD:
        /* code */
        sendCmd(sockfd, pcmdArg);
        break;
    default:
        return -1; // 未知命令
    }
}