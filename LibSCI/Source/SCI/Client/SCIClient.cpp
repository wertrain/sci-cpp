#include <thread>

#include <iostream>
#include <SCI/Client/SCIClient.h>

namespace sci
{

class SCIClient::Impl
{
public:
    bool Connect();
    void Proc();
};

bool SCIClient::Impl::Connect()
{
    std::thread th(&SCIClient::Impl::Proc, this);

    if (!th.joinable())
    {
        return false;
    }

    th.join();
    return true;
}

void SCIClient::Impl::Proc()
{
    int count = 0;
    while (1)
    {
        ++count;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << count << std::endl;
    }
}

SCIClient::SCIClient()
    : mImpl(new SCIClient::Impl)
{

}

SCIClient::~SCIClient()
{

}

bool SCIClient::Connect()
{
    return mImpl->Connect();
}

}; // namespace sci
