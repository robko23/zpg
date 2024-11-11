#ifndef LOG_GUARD
#define LOG_GUARD

#include <stdio.h>
#include <stdarg.h>

void __log(const char *level, const char *format, ...) {
    int pid = getpid();
    printf("%d: [ %s ] ", pid, level);
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 1
#define LOG_LEVEL_ERROR 0

#define LOG_LEVEL LOG_LEVEL_DEBUG

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(fmt, ...) __log("DEBUG", fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt,...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(fmt, ...) __log("INFO ", fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt,...)
#endif


#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOG_WARN(fmt, ...) __log("WARN ", fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt,...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(fmt, ...) __log("ERROR", fmt, ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt,...)
#endif

#endif