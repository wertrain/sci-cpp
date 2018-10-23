/**
 * @file SCIUtility.h
 * @brief ユーティリティ
 */
#pragma once

#include <memory>

NS_SCI_UT_BEGIN

void logging(const char* format, ...);

static void(*const error)(const char* format, ...) = logging;

NS_SCI_UT_END
