/**
 * @file SCIServer.h
 * @brief サーバを表すクラス
 */
#pragma once

#include<memory>

namespace sci
{

/// サーバを表すクラス
class SCIServer
{
public:
    SCIServer();
    virtual ~SCIServer();

    bool Start(const int port, const char* address);
    bool End();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
