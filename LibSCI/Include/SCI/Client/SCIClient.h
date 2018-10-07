#pragma once

#include<memory>

namespace sci
{

/// クライアントを表すクラス
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
