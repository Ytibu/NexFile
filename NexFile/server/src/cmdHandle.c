#include "../include/cmdHandle.h"

#include "../../shared/protocol.h"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>

static char currentDir[256] = "/home/dingjr/DevCode/NexFile/tmp/netDisk/";

int cmdParse(int sockFd, packetCmd_t *cmd)
{
    printf("will Parser\n");
    switch (cmd->cmdCode_)
    {
    case REQ_CD:
        changeDir(sockFd, cmd->data_);
        break;
    case REQ_LS:
        listDir(sockFd, cmd->data_);
        break;
    case REQ_MKDIR:
        makeDir(sockFd, cmd->data_);
        break;
    case REQ_RM:
        removeFile(sockFd, cmd->data_);
        break;
    case REQ_PUT:
        PutFile(sockFd, cmd->data_);
        break;
    case REQ_GET:
        GetFile(sockFd, cmd->data_);
        break;
    case REQ_PWD:
        printWorkingDir(sockFd);
        break;
    default:
        return -1;
        break;
    }

    return 0;
}

int listDir(int sockFd, const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        printf("%s\n", entry->d_name);
        
    }
}

int makeDir(int sockFd, const char *path)
{
    int result = mkdir(path, 0755);
    if(result == -1){
        perror("mkdir");
        return -1;
    }
    return result;
}
int removeFile(int sockFd, const char *path)
{
    int result = unlink(path);
    if(result == -1){
        perror("remove");
        return -1;
    }
    return 0;
}
int PutFile(int sockFd, const char *path)
{
    return 0;
}
int GetFile(int sockFd, const char *path)
{
    return 0;
}
int printWorkingDir(int sockFd)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return -1;
    }

    return 0;
}