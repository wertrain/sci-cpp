/**
 * @file SCIUtility.cpp
 * @brief ユーティリティ
 */
#include <Precompiled.h>
#include <stdarg.h>

#include <cstdio>
#ifdef _WIN64
#include <Windows.h>
#endif // _WIN64
#include <SCI/System/SCIUtility.h>

NS_SCI_UT_BEGIN

static uint32_t sEnableLoggingLevel = LogLevel::All;

void std_logging(const char* format, ...)
{
    va_list arg;

    const size_t size = 1024;
    char buffer[size];

    va_start(arg, format);
    vsnprintf(buffer, size, format, arg);
    va_end(arg);

    printf(buffer);
}

void std_logging_with_level(const LogLevel level, const char* format, va_list arg)
{
    if ((sEnableLoggingLevel & level) == 0) return;

    const size_t size = 1024;
    char buffer[size];

    vsnprintf(buffer, size, format, arg);

    printf(buffer);
}

void std_logging_debug(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    std_logging_with_level(LogLevel::Debug, format, arg);
    va_end(arg);
}

void std_logging_info(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    std_logging_with_level(LogLevel::Info, format, arg);
    va_end(arg);
}

void std_logging_caution(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    std_logging_with_level(LogLevel::Caution, format, arg);
    va_end(arg);
}

void std_logging_danger(const char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    std_logging_with_level(LogLevel::Danger , format, arg);
    va_end(arg);
}


void debug_logging(const char* format, ...)
{
    va_list arg;

    const size_t size = 1024;
    char buffer[size];

    va_start(arg, format);
    vsnprintf(buffer, size, format, arg);
    va_end(arg);

#ifdef _WIN64
    OutputDebugStringA(buffer);
#endif // _WIN64
}

NS_SCI_UT_END
