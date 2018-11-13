/**
 * @file SCIUtility.h
 * @brief ユーティリティ
 */
#pragma once

#include <memory>

NS_SCI_UT_BEGIN

void std_logging(const char* format, ...); 
void debug_logging(const char* format, ...);

static void(*const logging)(const char* format, ...) = std_logging;
static void(*const error)(const char* format, ...) = std_logging;

NS_SCI_UT_END
