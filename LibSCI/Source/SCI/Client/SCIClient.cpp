/**
 * @file SCIClient.cpp
 * @brief クライアントを表すクラス
 */
#include <Precompiled.h>

#include <stdint.h>
#include <thread>

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/Client/SCIClient.h>

namespace sci
{

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

class SCIClient::Impl
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

SCIClient::Impl::Impl()
    : mStatus(ConnectionStatus::NONE)
    , mSocket(INVALID_SOCKET)
{

}

SCIClient::Impl::~Impl()
{
    Disconnect();
}

bool SCIClient::Impl::Connect(const int port, const char* address)
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
    // inet_addr() の警告の対処
    // https://qiita.com/parallax_kk/items/9e877542fecb4087729f
    InetPtonA(addr.sin_family, address, &addr.sin_addr.S_un.S_addr);

    if (connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cout << "socket connect error. (" << WSAGetLastError() << ")" << std::endl;
        return false;
    }

    std::thread th(&SCIClient::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        return false;
    }

    th.join();

    mStatus = ConnectionStatus::CONNECTED;

    return true;
}

bool SCIClient::Impl::Disconnect()
{
    if (mSocket == INVALID_SOCKET)
    {
        return false;
    }
    else
    {
        closesocket(mSocket);
    }
    return true;
}

int SCIClient::Impl::Send(const char* buffer, const size_t bufferSize)
{
    return send(mSocket, buffer, static_cast<int>(bufferSize), 0);
}

SCIClient::Impl::ConnectionStatus SCIClient::Impl::GetStatus() const
{
    return mStatus;
}

int SCIClient::Impl::GetLastError() const
{
    return WSAGetLastError();
}

void SCIClient::Impl::Proc(long long intervalOfTime)
{
    int count = 0;
    while (true)
    {
        ++count;
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalOfTime));
        std::cout << count << std::endl;

        char send_buffer[64] = { "hello"};
        if (int len = send(mSocket, send_buffer, static_cast<int>(strlen(send_buffer)) + 1, 0) > 0)
        {
            std::cout << len << std::endl;
        }

        if (mStatus == TRY_DISCONNECT)
        {
            mStatus = DISCONNECTED;
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------

SCIClient::SCIClient()
    : mImpl(new SCIClient::Impl)
{

}

SCIClient::~SCIClient()
{

}

bool SCIClient::Connect(const int port, const char* address)
{
    return mImpl->Connect(port, address);
}

bool SCIClient::Disconnect()
{
    return mImpl->Disconnect();
}

int SCIClient::Send(const char* buffer, const size_t bufferSize)
{
    return mImpl->Send(buffer, bufferSize);
}

}; // namespace sci
