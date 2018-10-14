/**
 * @file SCIConnecter.h
 * @brief 接続を表すクラス
 */
#pragma once

#include <memory>

NS_SCI_SYS_BEGIN

/// 接続を表すクラス
class SCIConnecter
{
public:
    SCIConnecter();
    ~SCIConnecter();

    bool Connect(const int port, const char* address);
    bool Disconnect();
    int Send(const char* buffer, const size_t bufferSize);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

NS_SCI_SYS_END
