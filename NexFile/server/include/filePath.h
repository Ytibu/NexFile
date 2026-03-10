#ifndef __FILEPATH_H__
#define __FILEPATH_H__

typedef struct FilePath_s
{
    char *path_;    // 存储路径字符串
    int top_;       // 栈顶索引，初始值为1，表示路径栈中有一个元素（根目录）
} FilePath_t;

FilePath_t *initFilePath();
int pushFilePath(FilePath_t *filePath, const char *dirName);
int popFilePath(FilePath_t *filePath);
#endif