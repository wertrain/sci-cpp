#pragma once

#include<memory>

namespace sci
{

/// サーバを表すクラス
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
