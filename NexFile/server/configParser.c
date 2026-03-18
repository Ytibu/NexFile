#include "configParser.h"
#include  "../shared/logger.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

static Config_t globalConfig;
static int configInitialized = 0;

Config_t *getConfig()
{   if(configInitialized == 0) {
        parseConfig("server.conf", &globalConfig);
        return NULL;
    }
    return &globalConfig;
}

struct sockaddr_in Config(Config_t *config)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config->port);
    server_addr.sin_addr.s_addr = config->ipv4;
    return server_addr;
}

int parseConfig(const char *configPath, Config_t *config)
{
    int fd = open(configPath, O_RDONLY);
    ERROR_CHECK(fd, -1, "open config file");

    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    ERROR_CHECK(bytesRead, -1, "read config file");
    buffer[bytesRead] = '\0';

    close(fd);

    char ipStr[64] = {0};
    int port = 0;
    int threadNum = 0;

    char *saveptr = NULL;
    char *line = strtok_r(buffer, "\r\n", &saveptr);
    while (line) {
        if (sscanf(line, "ip: %63s", ipStr) == 1) {
            // parsed ip
        } else if (sscanf(line, "port: %d", &port) == 1) {
            // parsed port
        } else if (sscanf(line, "threadNum: %d", &threadNum) == 1) {
            // parsed thread number
        }
        line = strtok_r(NULL, "\r\n", &saveptr);
    }

    struct in_addr addr;
    int ret = inet_pton(AF_INET, ipStr, &addr);
    ERROR_CHECK(ret, 0, "invalid ip in config");
    ERROR_CHECK(ret, -1, "inet_pton failed");

    if (port <= 0 || port > 65535) {
        errno = EINVAL;
        ERROR_CHECK(-1, -1, "invalid port in config");
    }

    if (threadNum <= 0) {
        errno = EINVAL;
        ERROR_CHECK(-1, -1, "invalid threadNum in config");
    }

    // 初始化Config_t
    config->ipv4 = addr.s_addr; // 网络字节序
    config->port = port;
    config->threadNum = threadNum;

    if (config->configPath) {
        free(config->configPath);
        config->configPath = NULL;
    }
    config->configPath = strdup(configPath);
    ERROR_CHECK(config->configPath == NULL, 1, "strdup configPath");

    return 0;
}