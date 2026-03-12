#ifndef __CLIENT_SEND_MESSAGE_H__
#define __CLIENT_SEND_MESSAGE_H__

int recvn(int sockFd, void *buf, long total);
int UserAuthen(int sockfd);
int sendPassword(int fd, const char *username, const char *password);

#endif