#ifndef __SEND_MESSAGE_H__
#define __SEND_MESSAGE_H__

enum
{
    AUTH_RESULT_OK = 0,
    AUTH_RESULT_FAIL = -1,             // 用户名/密码错误，可重试
    AUTH_RESULT_USER_QUERY_FAIL = 1,   // 查询用户信息失败，可重试
    AUTH_RESULT_IO_ERROR = -2          // 连接断开或认证读写异常，不可重试
};

int recvn(int sockFd, void *buf, long total);
int UserAuthen(int sockfd);


#endif
