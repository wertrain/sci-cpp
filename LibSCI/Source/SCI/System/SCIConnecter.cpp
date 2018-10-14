/**
 * @file SCIConnecter.cpp
 * @brief ê⁄ë±Çï\Ç∑ÉNÉâÉX
 */
#include <Precompiled.h>

#include <stdint.h>
#include <thread>

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/System/SCIUtility.h>
#include <SCI/System/SCIConnecter.h>

NS_SCI_SYS_BEGIN

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

class SCIConnecter::Impl
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
    bool Listen(const int port, const char* address);
    bool Disconnect();
    void Proc(long long intervalOfTime);
    int Send(const char* buffer, const size_t bufferSize);
    ConnectionStatus GetStatus() const;
    int GetLastError() const;

private:
    ConnectionStatus mStatus;
    SOCKET mSocket;
};

SCIConnecter::Impl::Impl()
    : mStatus(ConnectionStatus::NONE)
    , mSocket(INVALID_SOCKET)
{

}

SCIConnecter::Impl::~Impl()
{
    Disconnect();
}

bool SCIConnecter::Impl::Connect(const int port, const char* address)
{
    mStatus = ConnectionStatus::TRY_CONNECT;

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        sci::ut::logging("socket failure. (%u)\n", WSAGetLastError());
        return false;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_addr() ÇÃåxçêÇÃëŒèà
    // https://qiita.com/parallax_kk/items/9e877542fecb4087729f
    InetPtonA(addr.sin_family, address, &addr.sin_addr.S_un.S_addr);

    if (connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        sci::ut::logging("socket connect failure. (%u)\n", WSAGetLastError());
        return false;
    }

    std::thread th(&SCIConnecter::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        return false;
    }

    th.join();

    mStatus = ConnectionStatus::CONNECTED;

    return true;
}

bool SCIConnecter::Impl::Listen(const int port, const char* address)
{
    mStatus = ConnectionStatus::TRY_CONNECT;

    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        sci::ut::logging("socket failure. (%u)\n", WSAGetLastError());
        return false;
    }
    struct sockaddr_in addr = { 0 };

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_addr() ÇÃåxçêÇÃëŒèà
    // https://qiita.com/parallax_kk/items/9e877542fecb4087729f
    InetPtonA(addr.sin_family, address, &addr.sin_addr.S_un.S_addr);

    if (int error = bind(mSocket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        sci::ut::logging("socket bind failure. (%u)\n", WSAGetLastError());
        return false;
    }

    const int backlog = 1;
    if (int error = listen(mSocket, backlog))
    {
        sci::ut::logging("socket listen failure. (%u)\n", WSAGetLastError());
        return false;
    }

    std::thread th(&SCIConnecter::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        return false;
    }

    th.join();

    mStatus = ConnectionStatus::CONNECTED;

    return true;
}

bool SCIConnecter::Impl::Disconnect()
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

int SCIConnecter::Impl::Send(const char* buffer, const size_t bufferSize)
{
    return send(mSocket, buffer, static_cast<int>(bufferSize), 0);
}

SCIConnecter::Impl::ConnectionStatus SCIConnecter::Impl::GetStatus() const
{
    return mStatus;
}

int SCIConnecter::Impl::GetLastError() const
{
    return WSAGetLastError();
}

void SCIConnecter::Impl::Proc(long long intervalOfTime)
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

SCIConnecter::SCIConnecter()
    : mImpl(new SCIConnecter::Impl)
{

}

SCIConnecter::~SCIConnecter()
{

}

bool SCIConnecter::Connect(const int port, const char* address)
{
    return mImpl->Connect(port, address);
}

bool SCIConnecter::Disconnect()
{
    return mImpl->Disconnect();
}

int SCIConnecter::Send(const char* buffer, const size_t bufferSize)
{
    return mImpl->Send(buffer, bufferSize);
}

NS_SCI_SYS_END
