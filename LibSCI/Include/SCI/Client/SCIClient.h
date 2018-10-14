/**
 * @file SCIClient.h
 * @brief クライアントを表すクラス
 */
#pragma once

#include<memory>

namespace sci
{

/// クライアントを表すクラス
class SCIClient
{
public:
    SCIClient();
    ~SCIClient();

    bool Connect(const int port, const char* address);
    bool Disconnect();
    int Send(const char* buffer, const size_t bufferSize);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
