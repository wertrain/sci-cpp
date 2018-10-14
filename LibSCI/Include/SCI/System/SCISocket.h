/**
 * @file SCISocket.h
 * @brief �\�P�b�g��\���N���X
 */
#pragma once

#include <memory>

NS_SCI_SYS_BEGIN

/// �\�P�b�g��\���N���X
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
