/**
 * @file SCIServer.cpp
 * @brief サーバを表すクラス
 */
#include <Precompiled.h>

#include <thread>
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <vector>

#include <SCI/Server/SCIServer.h>

namespace sci
{

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

class SCIServer::Impl
{
public:
    Impl();
    ~Impl();
    bool Connect(const int port, const char* address);
    bool Disconnect();
    void Proc(long long intervalOfTime);

private:
    struct Client
    {
    public:
        Client::Client()
            : mAddr({0})
            , mSocket(INVALID_SOCKET)
        {

        }
        const struct sockaddr_in& GetSockAddrIn() { return mAddr; }
        const SOCKET& GetSocket() { return mSocket; }
        void SetSockAddrIn(struct sockaddr_in addr) { mAddr = addr; }
        void SetSocket(SOCKET socket) { mSocket = socket; }

    private:
        struct sockaddr_in mAddr;
        SOCKET mSocket;
    };
    static const size_t MAX_CLIENT_NUM = 8;

private:
    SOCKET mSocket;
    int32_t mClientCount;
    std::vector<Client*> mClientList;
};

SCIServer::Impl::Impl()
    : mSocket(INVALID_SOCKET)
    , mClientCount(0)
    , mClientList()
{

}

SCIServer::Impl::~Impl()
{
    Disconnect();
}

bool SCIServer::Impl::Connect(const int port, const char* address)
{
    mSocket = socket(PF_INET, SOCK_STREAM, 0);
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
    while (true)
    {
        struct sockaddr_in addr = { 0 };
        int len = sizeof(addr);

        std::cout << "wait connection. please start client." << std::endl;
        SOCKET sockclient = accept(mSocket, (struct sockaddr *)&addr, &len);

        Client* client = new Client();
        client->SetSockAddrIn(addr);
        client->SetSocket(sockclient);
        mClientList.push_back(client);

        printf("%s から接続を受けました\n", inet_ntoa(addr.sin_addr));

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
