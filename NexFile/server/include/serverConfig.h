#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#include <sys/socket.h>

// 外置的配置文件路径宏定义
#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "server_config.xml"
#endif

// 服务器网络地址结构体宏定义
#ifndef SERVER_FAMILY
#define SERVER_FAMILY AF_INET
#endif
#ifndef SERVER_IP
#define SERVER_IP "0.0.0.0"
#endif
#ifndef SERVER_PORT
#define SERVER_PORT 1234
#endif

// 配置宏定义的线程数量和服务器文件目录
#ifndef WORKER_THREAD_NUM
#define WORKER_THREAD_NUM 4
#endif
#ifndef SERVER_FILE_PATH
#define SERVER_FILE_PATH "./server_files"
#endif

#endif // __SERVER_CONFIG_H__