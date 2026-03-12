#include "../include/fileOpt.h"

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int changeDir(packetCmd_t *packetCmd)
{
    if(packetCmd->data_ == NULL)
    {
        return 0;
    }
} 

int listDir(packetCmd_t *packetCmd)
{}

int printDir(packetCmd_t *packetCmd)
{}

int getFiles(packetCmd_t *packetCmd)
{
    
}

int putsFile(packetCmd_t *packetCmd)
{}

int removeFile(packetCmd_t *packetCmd)
{
    int unlink_ret = unlink(packetCmd->data_);
    if(unlink_ret == -1){
        perror("unlink");
        return -1;
    }

    return 0;
}

int makeDir(packetCmd_t *packetCmd)
{
    int mkd_ret = mkdir(packetCmd->data_, 0755);
    if(mkd_ret == -1)
    {
        perror("mkdir failed");
        return -1;
    }
    return 0;
}

int choiceFileOpt(packetCmd_t *packetCmd)
{
    switch (packetCmd->cmdCode_)
    {
    case REQ_CD:
        return changeDir(packetCmd);
    case REQ_LS:
        return listDir(packetCmd);
    case REQ_PWD:
        return printDir(packetCmd);
    case REQ_GET:
        return getFiles(packetCmd);
    case REQ_PUT:
        return putsFile(packetCmd);
    case REQ_RM:
        return removeFile(packetCmd);
    case REQ_MKDIR:
        return makeDir(packetCmd);
    default:
        fprintf(stderr, "Invalid command code: %u\n", packetCmd->cmdCode_);
        return -1;
    }
}