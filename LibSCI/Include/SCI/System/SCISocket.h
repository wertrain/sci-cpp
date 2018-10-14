/**
 * @file SCISocket.h
 * @brief ソケットを表すクラス
 */
#pragma once

#include <memory>

NS_SCI_SYS_BEGIN

/// ソケットを表すクラス
class SCISocket
{
public:
    SCISocket();
    ~SCISocket();
    bool Create();
    void Destroy();
private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

NS_SCI_SYS_END
