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

    bool Connect();

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}; // namespace sci
