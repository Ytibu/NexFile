#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

#define LOG_COLOR_RESET   "\x1b[0m"
#define LOG_COLOR_FATAL   "\x1b[1;35m"
#define LOG_COLOR_ERROR   "\x1b[1;31m"
#define LOG_COLOR_INFO    "\x1b[1;32m"
#define LOG_COLOR_DEBUG   "\x1b[1;36m"

#define SYSLOG(fmt, ...) \
    do { \
        pid_t _syslog_pid = fork(); \
        if (_syslog_pid == 0) { \
            if (setsid() < 0) { \
                _exit(1); \
            } \
            pid_t _syslog_pid2 = fork(); \
            if (_syslog_pid2 < 0) { \
                _exit(1); \
            } \
            if (_syslog_pid2 > 0) { \
                _exit(0); \
            } \
            openlog("NexFile", LOG_PID | LOG_NDELAY, LOG_USER); \
            syslog(LOG_INFO, "| %-5s | %s:%s:%d | " fmt, \
                   "INFO", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
            closelog(); \
            _exit(0); \
        } else if (_syslog_pid > 0) { \
            (void)waitpid(_syslog_pid, NULL, 0); \
        } else { \
            openlog("NexFile", LOG_PID | LOG_NDELAY, LOG_USER); \
            syslog(LOG_INFO, "| %-5s | %s:%s:%d | " fmt, \
                   "INFO", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
            closelog(); \
        } \
    } while (0)

#define ERROR_CHECK(num, expect, message)                     \
               do                                                        \
               {                                                         \
                   if ((num) == (expect))                                \
                   {                                                     \
                       fprintf(stderr, LOG_COLOR_ERROR "[ERROR]" LOG_COLOR_RESET " %s:%d %s() | %s \n", \
                               __FILE__, __LINE__, __func__, (message)); \
                       exit(1);                                          \
                   }                                                     \
               } while (0)

#define ARGC_CHECK(argc, num, message)        \
               do                                        \
               {                                         \
                   if (argc != num)                      \
                   {                                     \
                       fprintf(stderr, "%s\n", message); \
                       exit(1);                          \
                   }                                     \
               } while (0)

#define LOG_FATAL(fmt, ...) \
    do{\
        fprintf(stderr, \
        LOG_COLOR_FATAL "[FATAL]" LOG_COLOR_RESET " %s:%d %s() | " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
        exit(1);\
    } while (0)

#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, LOG_COLOR_ERROR "[ERROR]" LOG_COLOR_RESET " %s:%d %s() | " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LOGINFO(fmt, ...) \
    fprintf(stdout, LOG_COLOR_INFO "[INFO]" LOG_COLOR_RESET " %s:%d %s() | " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#ifdef LOG_DEBUG
#undef LOG_DEBUG
#endif
#define LOG_DEBUG(fmt, ...) \
    fprintf(stdout, LOG_COLOR_DEBUG "[DEBUG]" LOG_COLOR_RESET " %s:%d %s() | " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif