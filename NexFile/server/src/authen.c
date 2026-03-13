#include "../include/authen.h"

#include <stdio.h>
#include <shadow.h>
#include <crypt.h>
#include <string.h>

// 用户密码对比：0表示认证成功，-1表示认证失败，1表示查询用户信息失败
int authenticateUser(const char *username, const char *cryptPassword)
{
    if(username == NULL || cryptPassword == NULL)
    {
        return 0; // 认证失败
    }
    struct spwd * pinfo = getspnam(username);   // 获取用户信息，校验用户是否存在
    if(pinfo == NULL)
    {
        perror("getspnam");
        return 1; //1表示查询用户信息失败
    }
    int result = strcmp(cryptPassword, pinfo->sp_pwdp); // 比较密码，返回0表示匹配成功
    if(result != 0)
    {
        return -1; // 认证失败
    }
    printf("User %s authenticated successfully.\n", username);
    return 0; // 返回1表示认证成功，0表示认证失败
}

