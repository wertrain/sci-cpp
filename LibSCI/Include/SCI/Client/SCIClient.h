/**
 * @file SCIClient.h
 * @brief �N���C�A���g��\���N���X
 */
#pragma once

#include<memory>

namespace sci
{

/// �N���C�A���g��\���N���X
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
