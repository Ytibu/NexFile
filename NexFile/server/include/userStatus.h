#ifndef __USERSTATUS_H__
#define __USERSTATUS_H__

#include <string.h>
#include <stdio.h>

typedef struct userStatus_s
{
    char username[32]; // 用户名
    char currentDir[256]; // 当前目录
    int isAuthenticated; // 是否已认证
} userStatus_t;

int initUserStatus(userStatus_t *status, const char *username);
#endif //__USERSTATUS_H__