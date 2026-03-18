#ifndef __EPOLL_H__
#define __EPOLL_H__


int epollADD(int epollFd, int fd);
int epollDEL(int epollFd, int fd);

#endif