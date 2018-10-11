#include <stdint.h>
#include <thread>

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/System/SCISocket.h>

namespace sci
{

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

class SCISocket::Impl
{
public:
    enum ConnectionStatus : uint32_t
    {
        NONE = 0,
        IDLE,
        TRY_CONNECT,
        CONNECTED,
        TRY_DISCONNECT,
        DISCONNECTED
    };

public:
    Impl();
    ~Impl();
    bool Connect(const int port, const char* address);
    bool Disconnect();
    void Proc(long long intervalOfTime);
    int Send(const char* buffer, const size_t bufferSize);
    ConnectionStatus GetStatus() const;
    int GetLastError() const;

private:
    ConnectionStatus mStatus;
    SOCKET mSocket;
};

SCISocket::Impl::Impl()
    : mStatus(ConnectionStatus::NONE)
    , mSocket(INVALID_SOCKET)
{

}

SCISocket::Impl::~Impl()
{
    Disconnect();
}

bool SCISocket::Impl::Connect(const int port, const char* address)
{
    mStatus = ConnectionStatus::TRY_CONNECT;

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        std::cout << "socket failure. (" << WSAGetLastError() << ")" << std::endl;
        return false;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_addr() ‚ÌŒx‚Ì‘Îˆ
    // https://qiita.com/parallax_kk/items/9e877542fecb4087729f
    InetPtonA(addr.sin_family, address, &addr.sin_addr.S_un.S_addr);

    if (connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cout << "socket connect error. (" << WSAGetLastError() << ")" << std::endl;
        return false;
    }

    std::thread th(&SCISocket::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        return false;
    }

    th.join();

    mStatus = ConnectionStatus::CONNECTED;

    return true;
}

bool SCISocket::Impl::Disconnect()
{
    if (mSocket == INVALID_SOCKET)
    {
        return false;
    }
    else
    {
        closesocket(mSocket);
    }
}

int SCISocket::Impl::Send(const char* buffer, const size_t bufferSize)
{
    return send(mSocket, buffer, bufferSize, 0);
}

SCISocket::Impl::ConnectionStatus SCISocket::Impl::GetStatus() const
{
    return mStatus;
}

int SCISocket::Impl::GetLastError() const
{
    return WSAGetLastError();
}

//-------------------------------------------------------------------------------------------------

SCISocket::SCISocket()
    : mImpl(new SCISocket::Impl)
{

}

SCISocket::~SCISocket()
{

}

bool SCISocket::Connect(const int port, const char* address)
{
    return mImpl->Connect(port, address);
}

bool SCISocket::Disconnect()
{
    return mImpl->Disconnect();
}

int SCISocket::Send(const char* buffer, const size_t bufferSize)
{
    return mImpl->Send(buffer, bufferSize);
}

}; // namespace sci
