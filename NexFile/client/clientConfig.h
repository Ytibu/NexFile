#ifndef __CLIENT_CONFIG_H__
#define __CLIENT_CONFIG_H__

#include <sys/socket.h>

//宏定义项目的配置
#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "client_config.xml"
#endif

// 客户端网络地址结构体宏定义
#ifndef CLIENT_FAMILY
#define CLIENT_FAMILY AF_INET
#endif
#ifndef CLIENT_IP
#define CLIENT_IP "127.0.0.1"
#endif
#ifndef CLIENT_PORT
#define CLIENT_PORT 1234
#endif

// 客户端文件目录宏定义
#ifndef CLIENT_FILE_PATH
#define CLIENT_FILE_PATH "/home/dingjr/DevCode/NexFile/tmp/netDisk"
#endif

#endif // __CLIENT_CONFIG_H__