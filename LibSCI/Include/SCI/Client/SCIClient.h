#pragma once

#include<memory>

namespace sci
{

/// �N���C�A���g��\���N���X
class SCIClient
{
public:
    SCIClient();
    virtual ~SCIClient();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
