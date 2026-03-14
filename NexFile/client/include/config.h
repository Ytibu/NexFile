#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <netinet/in.h>

typedef struct Config_s {
    struct sockaddr_in addr; // 服务器地址结构体
    char filePath[1024];     // 本地文件目录
    char configPath[1024];   // 配置文件路径
} Config_t;

// 全局客户端配置，初始化后可在其他模块直接读取
extern Config_t g_clientConfig;

// 解读配置文件，填充Config_t结构体
int Config(const char *configPath, Config_t *config);

// 加载配置到全局对象 g_clientConfig
int LoadGlobalConfig(const char *configPath);

// 获取全局配置只读视图
const Config_t *GetGlobalConfig(void);

// 打印配置内容（调试用）
void printClientConfig(Config_t *config);

#endif // Config_t