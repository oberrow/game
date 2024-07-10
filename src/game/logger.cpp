/*
 * game/logger.cpp
 *
 * Copyright (c) 2024 Omar Berrow
*/

#include <cstdarg>
#include <cstddef>
#include <mutex>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include <logger.h>

namespace logger
{
    static log_level s_logLevel;
    void SetLogLevel(log_level to)
    {
        s_logLevel = to;
    }
    size_t internal_log(log_level level, const char* log_prefix, const char* format, va_list list)
    {
        if (level < s_logLevel)
            return 0;
        static std::mutex lock;
        lock.lock();
        size_t ret = 0;
        ret += fprintf(stderr, "%s ", log_prefix);
        ret += vfprintf(stderr, format, list);
        ret += fprintf(stderr, "\x1b[0m");
        lock.unlock();
        return ret;
    }
    size_t Debug(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        size_t ret = internal_log(log_level::Debug, "\x1b[34m[ DEBUG ]", format, list); // Blue
        va_end(list);
        return ret;
    }
    size_t Log(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        size_t ret = internal_log(log_level::Log, "\x1b[32m[ LOG ]", format, list); // Green
        va_end(list);
        return ret;
    }
    size_t Warning(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        size_t ret = internal_log(log_level::Warning, "\x1b[33m[ WARN ]", format, list); // Yellow
        va_end(list);
        return ret;
    }
    size_t Error(const char* format, ...)
    {
        va_list list;
        va_start(list, format);
        size_t ret = internal_log(log_level::Error, "\x1b[31m[ ERROR ]", format, list); // Red
        va_end(list);
        return ret;
    }
}