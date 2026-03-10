#ifndef __FILEOPT_H__
#define __FILEOPT_H__
#include "../../shared/protocol.h"

int changeDir(PacketCmd_t *packetCmd);

int listDir(PacketCmd_t *packetCmd);

int printDir(PacketCmd_t *packetCmd);

int getFiles(PacketCmd_t *packetCmd);

int putsFile(PacketCmd_t *packetCmd);

int removeFile(PacketCmd_t *packetCmd);

int makeDir(PacketCmd_t *packetCmd);

int choiceFileOpt(PacketCmd_t *packetCmd);

#endif