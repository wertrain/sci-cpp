#pragma once

#include<memory>

namespace sci
{

/// �\�P�b�g��\���N���X
class SCISocket
{
public:
    SCISocket();
    ~SCISocket();

    bool Connect(const int port, const char* address);
    bool Disconnect();
    int Send(const char* buffer, const size_t bufferSize);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
