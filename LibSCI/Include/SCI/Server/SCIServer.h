/**
 * @file SCIServer.h
 * @brief �T�[�o��\���N���X
 */
#pragma once

#include<memory>

namespace sci
{

/// �T�[�o��\���N���X
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
