#include "../include/authen.h"

#include <stdio.h>
#include <shadow.h>
#include <crypt.h>
#include <string.h>

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
        return -1; //-1表示查询用户信息失败
    }
    printf("Authenticating user: '%s'\n", pinfo->sp_namp); // 输出正在认证的用户名
    printf("Received encrypted password: '%s'\n", cryptPassword); // 输出接收到的加密密码
    printf("Stored encrypted password:   '%s'\n", pinfo->sp_pwdp); // 输出存储的加密密码
    int result = strcmp(cryptPassword, pinfo->sp_pwdp); // 比较密码，返回0表示匹配成功
    if(result != 0)
    {
        return 0; // 认证失败
    }
    return 1; // 返回1表示认证成功，0表示认证失败
}

