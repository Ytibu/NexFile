#include "../include/encryption.h"
#include "../include/status.h"
#include "../include/clientsendMessage.h"
#include "../../shared/logger.h"
#include "../../shared/protocol.h"

#include <stdio.h>
#include <shadow.h>
#include <crypt.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

// 使用 crypt 函数加密密码
char *encryptPassword(const char *Password)
{
    char salt[] = "$y$j9T$H1C6NG.1YBywxNo4wQ409.$";
    char *hash = crypt(Password, salt);
    return hash;
}

// 向后端发送用户验证信息
int handlePassword(int sockfd, const char *username, const char *password)
{
    if (username == NULL || password == NULL)
    {
        return -1;
    }

    char *encryptedPassword = encryptPassword(password); // 加密密码

    // 成功为0，失败为-1
    ssize_t send_ret = sendPassword(sockfd, username, encryptedPassword); // 发送用户名和加密后的密码
    if (send_ret == -1)
    {
        LOG_ERROR("Failed to send authentication information to server\n");
        return -1;
    }

    return send_ret;
}

// 密码加密，认证信息发送，认证结果判断
int handle_authentication(int sockfd, const char *username, const char *password)
{
    if (username == NULL || password == NULL)
    {
        return -1;
    }

    if (handlePassword(sockfd, username, password) != 0) // 发送用户名和加密后的密码
    {
        return -1;
    }

    train_t train;
    char auth_response[64] = {0};
    recvn(sockfd, &train.length_, sizeof(int));
    recvn(sockfd, train.data_, train.length_);
    memcpy(auth_response, train.data_, train.length_);
    auth_response[train.length_] = '\0'; // 确保字符串以 null 结尾
    if (strcmp(auth_response, "AUTH_RESULT_OK") == 0)
    {
        printf("Authentication successful. You can now enter commands.\n");
        g_clientState.is_connected = 1;
    }
    else
    {
        printf("Authentication failed. Please try again.\n");
        return -1;
    }

    return 0;
}
