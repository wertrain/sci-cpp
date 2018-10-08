#include <thread>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <SCI/Server/SCIServer.h>

namespace sci
{

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

class SCIServer::Impl
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

private:
    ConnectionStatus mStatus;
    SOCKET mSocket;
};

SCIServer::Impl::Impl()
    : mStatus(ConnectionStatus::NONE)
    , mSocket(INVALID_SOCKET)
{

}

SCIServer::Impl::~Impl()
{
    Disconnect();
}

bool SCIServer::Impl::Connect(const int port, const char* address)
{
    mStatus = ConnectionStatus::TRY_CONNECT;

    mSocket = socket(PF_INET, SOCK_STREAM, 0);
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

    if (int error = bind(mSocket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        std::cout << "socket bind error. (" << WSAGetLastError() << ")" << std::endl;
        return false;
    }

    const int backlog = 1;
    if (int error = listen(mSocket, backlog))
    {
        std::cout << "socket listen error. (" << WSAGetLastError() << ")" << std::endl;
        return false;
    }

    std::thread th(&SCIServer::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        return false;
    }

    th.join();

    mStatus = ConnectionStatus::CONNECTED;

    return true;
}

bool SCIServer::Impl::Disconnect()
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

void SCIServer::Impl::Proc(long long intervalOfTime)
{
    struct sockaddr_in client;
    int len = sizeof(client);

    while (true)
    {
        std::cout << "wait connection. please start client." << std::endl;
        SOCKET sockclient = accept(mSocket, (struct sockaddr *)&client, &len);
        printf("%s ‚©‚çÚ‘±‚ðŽó‚¯‚Ü‚µ‚½\n", inet_ntoa(client.sin_addr));

        char buffer[1024];
        if (recv(sockclient, buffer, sizeof(buffer), 0) > 0)
        {
            std::cout << buffer << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalOfTime));
    }
}

//-------------------------------------------------------------------------------------------------

SCIServer::SCIServer()
    : mImpl(new SCIServer::Impl)
{

}

SCIServer::~SCIServer()
{

}

bool SCIServer::Start(const int port, const char* address)
{
    return mImpl->Connect(port, address);
}

bool SCIServer::End()
{
    return mImpl->Disconnect();
}

}; // namespace sci
