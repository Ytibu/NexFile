#include "../include/server/epoll.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>


int epollADD(int epollFd, int fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
}
int epollDEL(int epollFd, int fd)
{
    return epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
}