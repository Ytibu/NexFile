#ifndef __FILEOPT_H__
#define __FILEOPT_H__
#include "../../shared/protocol.h"

int changeDir(packetCmd_t *packetCmd);

int listDir(packetCmd_t *packetCmd);

int printDir(packetCmd_t *packetCmd);

int getFiles(packetCmd_t *packetCmd);

int putsFile(packetCmd_t *packetCmd);

int removeFile(packetCmd_t *packetCmd);

int makeDir(packetCmd_t *packetCmd);

int choiceFileOpt(packetCmd_t *packetCmd);

#endif