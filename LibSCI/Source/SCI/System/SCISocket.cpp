#include <Precompiled.h>

#include <stdint.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/System/SCIUtility.h>
#include <SCI/System/SCISocket.h>

namespace sci
{

namespace sys
{

class SCISocket::Impl
{
public:
    Impl();
    ~Impl();

    bool Create();
    void Destroy();
private:
    SOCKET mSocket;
};

SCISocket::Impl::Impl()
    : mSocket(INVALID_SOCKET)
{

}

SCISocket::Impl::~Impl()
{
    Destroy();
}

bool SCISocket::Impl::Create()
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        sci::ut::logging("socket failure. (%u)\n", WSAGetLastError());
        return false;
    }
    return true;
}

void SCISocket::Impl::Destroy()
{
    if (mSocket != INVALID_SOCKET)
    {
        closesocket(mSocket);
        mSocket = INVALID_SOCKET;
    }
}

//-------------------------------------------------------------------------------------------------

SCISocket::SCISocket()
    : mImpl(new SCISocket::Impl)
{

}

SCISocket::~SCISocket()
{

}

bool SCISocket::Create()
{
    return mImpl->Create();
}

void SCISocket::Destroy()
{
    mImpl->Destroy();
}

}; // namespace sys

}; // namespace sci
