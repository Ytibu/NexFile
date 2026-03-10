#include "../include/fileOpt.h"

#include <stdio.h>

int changeDir(PacketCmd_t *packetCmd)
{
    if(packetCmd->data_ == NULL)
    {
        
    }
}

int listDir(PacketCmd_t *packetCmd)
{}

int printDir(PacketCmd_t *packetCmd)
{}

int getFiles(PacketCmd_t *packetCmd)
{}

int putsFile(PacketCmd_t *packetCmd)
{}

int removeFile(PacketCmd_t *packetCmd)
{}

int makeDir(PacketCmd_t *packetCmd)
{}

int choiceFileOpt(PacketCmd_t *packetCmd)
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