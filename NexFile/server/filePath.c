#include "filePath.h"
#include  "../shared/protocol.h"

#include <stdlib.h>
#include <string.h>

FilePath_t *initFilePath()
{
    FilePath_t *p = (FilePath_t *)malloc(sizeof(FilePath_t));
    if (p == NULL)
    {
        return NULL;
    }

    p->path_ = (char *)malloc(2);
    if (p->path_ == NULL)
    {
        free(p);
        return NULL;
    }
    p->path_[0] = '/';
    p->path_[1] = '\0'; // 初始化路径为根目录
    p->top_ = 1;        // 栈顶索引初始化为1
    return p;
}

int pushFilePath(FilePath_t *filePath, const char *dirName)
{
    if (filePath == NULL || filePath->path_ == NULL || dirName == NULL)
    {
        return -1;
    }

    while (*dirName == '/')
    {
        ++dirName;
    }
    if (*dirName == '\0')
    {
        return -1;
    }

    size_t dirLen = strlen(dirName);
    while (dirLen > 0 && dirName[dirLen - 1] == '/')
    {
        --dirLen;
    }
    if (dirLen == 0)
    {
        return -1;
    }

    size_t pathLen = strlen(filePath->path_);
    int isRoot = (pathLen == 1 && filePath->path_[0] == '/');
    size_t newLen = pathLen + (isRoot ? 0 : 1) + dirLen;

    char *newPath = (char *)malloc(newLen + 1);
    if (newPath == NULL)
    {
        return -1;
    }

    memcpy(newPath, filePath->path_, pathLen);
    size_t offset = pathLen;
    if (!isRoot)
    {
        newPath[offset++] = '/';
    }
    memcpy(newPath + offset, dirName, dirLen);
    offset += dirLen;
    newPath[offset] = '\0';

    free(filePath->path_);
    filePath->path_ = newPath;
    filePath->top_++;
    return 0;
}

int popFilePath(FilePath_t *filePath)
{
    if (filePath == NULL || filePath->path_ == NULL)
    {
        return -1;
    }

    if (filePath->top_ <= 1 || (filePath->path_[0] == '/' && filePath->path_[1] == '\0'))
    {
        return -1;
    }

    char *lastSlash = strrchr(filePath->path_, '/');
    if (lastSlash == NULL)
    {
        return -1;
    }

    if (lastSlash == filePath->path_)
    {
        filePath->path_[1] = '\0';
    }
    else
    {
        *lastSlash = '\0';
    }

    filePath->top_--;
    return 0;
}