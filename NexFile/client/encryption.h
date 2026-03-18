#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

// 使用 crypt 函数加密密码
char *encryptPassword(const char *Password);

// 调用加密函数并发送认证信息
int handlePassword(int sockfd, const char *username, const char *password);

// 密码加密，认证信息发送，认证结果判断
int handle_authentication(int sockfd, const char *username, const char *password);

#endif