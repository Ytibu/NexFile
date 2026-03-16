#ifndef __CMDHANDLE_H__
#define __CMDHANDLE_H__

#include "../../shared/protocol.h"

int cmdParse(int sockFd, packetCmd_t *cmd);

int listDir(int sockFd, const char *path);
int makeDir(int sockFd, const char *path);
int removeFile(int sockFd, const char *path);
int PutFile(int sockFd, const char *path);
int GetFile(int sockFd, const char *path);
int printWorkingDir(int sockFd);
#endif