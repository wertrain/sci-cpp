/**
 * @file SCIUtility.h
 * @brief ユーティリティ
 */
#pragma once

#include <memory>

NS_SCI_UT_BEGIN

enum LogLevel : uint8_t
{
    Debug   = 1 << 0,
    Info    = 1 << 1,
    Caution = 1 << 2,
    Warning = 1 << 3,
    Danger  = 1 << 4,
    All = 0xF
};

void std_logging(const char* format, ...);
void std_logging_debug(const char* format, ...);
void std_logging_info(const char* format, ...);
void std_logging_caution(const char* format, ...);
void std_logging_danger(const char* format, ...);

static void(*const logging)(const char* format, ...) = std_logging;
static void(*const error)(const char* format, ...) = std_logging; 
static void(*const debug)(const char* format, ...) = std_logging_debug;
static void(*const info)(const char* format, ...) = std_logging_info;
static void(*const caution)(const char* format, ...) = std_logging_caution;
static void(*const danger)(const char* format, ...) = std_logging_danger;

NS_SCI_UT_END
