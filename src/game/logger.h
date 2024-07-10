/*
 * game/logger.h
 *
 * Copyright (c) 2024 Omar Berrow
*/

#pragma once

#include <stddef.h>

namespace logger
{
    enum class log_level
    {
        Debug,
        Log,
        Warning,
        Error,
    };
    void SetLogLevel(log_level to);
    size_t Debug(const char* format, ...);
    size_t Log(const char* format, ...);
    size_t Warning(const char* format, ...);
    size_t Error(const char* format, ...);
}