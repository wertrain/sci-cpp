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

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
