#ifndef __FILEOPT_H__
#define __FILEOPT_H__

typedef struct FileOpt
{
    int fd;
    char *fileName;
    char *filePath;
    char *fileType;
    long fileSize;
} FileOpt;

int changeDir(const char *dirPath);

int listDir(const char *dirPath);

int printDir(const char *dirPath);

int getFiles(const char *dirPath);

int putsFile(FileOpt *fileOpt);

int removeFile(const char *filePath);

int makeDir(const char *dirPath);

#endif