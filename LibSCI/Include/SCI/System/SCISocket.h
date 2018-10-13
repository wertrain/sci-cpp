#pragma once

#include<memory>

namespace sci {

namespace sys {

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

}; // namespace sys

}; // namespace sci
