/**
 * @file SCI.h
 * @brief ライブラリ共通
 */
#pragma once

#include <SCI/Client/SCIClient.h>
#include <SCI/Server/SCIServer.h>

namespace sci
{
    /// ライブラリを初期化
    bool SCIInitialize();
    /// ライブラリを終了
    void SCIFinalize();
}
