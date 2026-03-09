#ifndef __CMDVERI_H__
#define __CMDVERI_H__

typedef struct cmdArg_s {
    char *cmd_;
    char *arg_;
} cmdArg_t;

int cmdCut(char *cmd, cmdArg_t *pcmdArg);  // 切割命令，返回切割后的参数数量
int cmdVeri(cmdArg_t *pcmdArg); // 验证命令是否合法，返回1合法，0不合法

int cmdCheck(char *cmd); // 综合切割和验证命令，返回1合法，0不合法

#endif