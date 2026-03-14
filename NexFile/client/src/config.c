#include "../include/config.h"

#include "../../shared/logger.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

Config_t g_clientConfig = {0};

// 解读配置文件，填充Config_t结构体
int Config(const char *configPath, Config_t *config) {
    if (configPath == NULL || config == NULL) {
        fprintf(stderr, "configPath or config is NULL\n");
        return -1;
    }

    FILE *fp = fopen(configPath, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char line[256];
    char ip[64] = {0};
    int port = 0;
    char filePath[256] = {0};

    memset(config, 0, sizeof(*config));

    while (fgets(line, sizeof(line), fp)) {
        // 去除换行符
        line[strcspn(line, "\n")] = '\0';

        if (sscanf(line, "ip: %63s", ip) == 1) {
            continue;
        }
        if (sscanf(line, "port: %d", &port) == 1) {
            continue;
        }
        if (sscanf(line, "filePath: %255s", filePath) == 1) {
            continue;
        }
    }
    fclose(fp);

    if (strlen(ip) == 0 || port == 0 || strlen(filePath) == 0) {
        fprintf(stderr, "配置文件缺少必要字段\n");
        return -1;
    }

    // 填充 addr 结构
    config->addr.sin_family = AF_INET;
    config->addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &config->addr.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    // 按结构体中的定长数组保存路径
    snprintf(config->filePath, sizeof(config->filePath), "%s", filePath);
    snprintf(config->configPath, sizeof(config->configPath), "%s", configPath);

    return 0;
}

int LoadGlobalConfig(const char *configPath)
{
    return Config(configPath, &g_clientConfig);
}

const Config_t *GetGlobalConfig(void)
{
    return &g_clientConfig;
}

// 打印配置内容（调试用）
void printClientConfig(Config_t *config) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &config->addr.sin_addr, ip, sizeof(ip));

    printf("服务器地址: %s\n", ip);
    printf("端口: %d\n", ntohs(config->addr.sin_port));
    printf("本地文件目录: %s\n", config->filePath);
    printf("配置文件路径: %s\n", config->configPath);
}