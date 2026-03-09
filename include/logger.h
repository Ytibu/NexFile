#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>

static inline void Daemon(void)
{
    // 新建会话
    if (fork() < 0) {
        exit(1);
    } else if (fork() > 0) {
        exit(0);
    }
    setsid();

    // 关闭设备
    for (int i = 0; i < 1023; ++i) {
        close(i);
    }

    // 修改环境属性
    chdir("/");
    umask(0);
}

static inline void SYSLOG_IMPL(const char *file, const char *func, int line, const char *fmt, ...)
{
    char message[1024];
    va_list args;

    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);

    // 写入到系统日志：时间 | 级别 | 文件:函数:行号 | 消息
    syslog(LOG_INFO,
           "| %-5s | %s:%s:%d | %s",
           "INFO", file, func, line, message);
}

// 重要内容写入 LOG_INFO 到 /var/log/syslog
#define SYSLOG(fmt, ...) \
    SYSLOG_IMPL(__FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)

// num为函数返回值，expect为期望错误值，message为错误信息
#define ERROR_CHECK(num, expect, message) do{ \
    if ((num) == (expect)) { \
        fprintf(stderr, "[ERROR] %s:%d %s() | %s | errno=%d(%s)\n", \
                __FILE__, __LINE__, __func__, (message), errno, strerror(errno)); \
        exit(1); \
    } \
} while(0)

#define ARGC_CHECK(argc, num, message) do{ \
    if(argc != num){ \
        fprintf(stderr, "%s\n", message); \
        exit(1); \
    } \
} while(0)

#endif