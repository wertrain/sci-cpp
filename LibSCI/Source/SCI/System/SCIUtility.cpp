/**
 * @file SCIUtility.cpp
 * @brief ユーティリティ
 */
#include <Precompiled.h>
#include <stdarg.h>

#ifdef _WIN64
#include <Windows.h>
#endif // _WIN64
#include <SCI/System/SCIUtility.h>

NS_SCI_UT_BEGIN

void logging(const char* format, ...)
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
