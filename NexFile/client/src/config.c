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

static void trimWhitespace(char *s)
{
    char *start = s;
    while (*start && isspace((unsigned char)*start)) {
        ++start;
    }

    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        --len;
    }
}

static int extractXmlTagValue(const char *line,
                              const char *tag,
                              char *out,
                              size_t outSize)
{
    if (!line || !tag || !out || outSize == 0) {
        return 0;
    }

    char openTag[64];
    char closeTag[64];
    int openLen = snprintf(openTag, sizeof(openTag), "<%s>", tag);
    int closeLen = snprintf(closeTag, sizeof(closeTag), "</%s>", tag);

    if (openLen <= 0 || closeLen <= 0 ||
        (size_t)openLen >= sizeof(openTag) ||
        (size_t)closeLen >= sizeof(closeTag)) {
        return 0;
    }

    const char *start = strstr(line, openTag);
    if (!start) {
        return 0;
    }
    start += (size_t)openLen;

    const char *end = strstr(start, closeTag);
    if (!end || end <= start) {
        return 0;
    }

    size_t valueLen = (size_t)(end - start);
    if (valueLen >= outSize) {
        valueLen = outSize - 1;
    }

    memcpy(out, start, valueLen);
    out[valueLen] = '\0';
    trimWhitespace(out);
    return 1;
}

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

    char line[512];
    char ip[64] = {0};
    char family[32] = {0};
    int port = 0;
    char filePath[1024] = {0};
    char valueBuf[1024];

    memset(config, 0, sizeof(*config));

    while (fgets(line, sizeof(line), fp)) {
        if (extractXmlTagValue(line, "family", valueBuf, sizeof(valueBuf))) {
            snprintf(family, sizeof(family), "%s", valueBuf);
            continue;
        }

        if (extractXmlTagValue(line, "ip", valueBuf, sizeof(valueBuf))) {
            snprintf(ip, sizeof(ip), "%s", valueBuf);
            continue;
        }

        if (extractXmlTagValue(line, "port", valueBuf, sizeof(valueBuf))) {
            char *endPtr = NULL;
            long parsedPort = strtol(valueBuf, &endPtr, 10);
            if (endPtr == valueBuf || *endPtr != '\0' || parsedPort <= 0 || parsedPort > 65535) {
                fprintf(stderr, "配置文件中的 port 非法: %s\n", valueBuf);
                fclose(fp);
                return -1;
            }
            port = (int)parsedPort;
            continue;
        }

        if (extractXmlTagValue(line, "localPath", valueBuf, sizeof(valueBuf))) {
            snprintf(filePath, sizeof(filePath), "%s", valueBuf);
            continue;
        }
    }
    fclose(fp);

    if (strlen(ip) == 0 || port == 0 || strlen(filePath) == 0) {
        fprintf(stderr, "配置文件缺少必要字段\n");
        return -1;
    }

    if (strlen(family) > 0 && strcmp(family, "AF_INET") != 0) {
        fprintf(stderr, "暂不支持的 family: %s\n", family);
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