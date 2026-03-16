#ifndef __FILEHANDLE_H__
#define __FILEHANDLE_H__

int recvFile(int sockFd);
int sendFile(int sockFd, const char* fileName);

#endif // __FILEHANDLE_H__