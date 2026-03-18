#include "userStatus.h"

int initUserStatus(userStatus_t *status, const char *username)
{
    if (status == NULL || username == NULL)
        return -1;

    strncpy(status->username, username, sizeof(status->username) - 1);
    status->username[sizeof(status->username) - 1] = '\0'; // 确保字符串结尾

    // 初始化当前目录为用户的主目录
    snprintf(status->currentDir, sizeof(status->currentDir), "/home/dingjr/DevCode/NexFile/tmp/netDisk/%s/", username);

    return 0;
}