#ifndef __CLIENT_SEND_MESSAGE_H__
#define __CLIENT_SEND_MESSAGE_H__

int sendn(int sockfd, const void *buf, long total);
int sendPassword(int fd, const char *username, const char *password);

#endif