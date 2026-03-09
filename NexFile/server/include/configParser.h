#ifndef __CONFIG_PARSER_H__
#define __CONFIG_PARSER_H__

#include <sys/socket.h>
#include <netinet/in.h>

typedef struct Config_s {
    int ipv4;
    int port;
    int threadNum;
    char *configPath;
} Config_t;

struct sockaddr_in Config(Config_t *config);

int parseConfig(const char *configPath, Config_t *config);

#endif // __CONFIG_PARSER_H__