/**
 * @file SCIClient.cpp
 * @brief クライアントを表すクラス
 */
#include <Precompiled.h>

#include <stdint.h>
#include <thread>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/System/SCIPacket.h>
#include <SCI/System/SCIUtility.h>
#include <SCI/Client/SCIClient.h>

namespace sci
{

static const long long INTERVAL_OF_PROCESS_TIME_MILLISECONDS = 1000 * 1;
static const long long INTERVAL_OF_RETRY_TIME_MILLISECONDS = 1000 * 5;

class SCIClient::Impl : public sys::SCIPacketSender
{
public:
    Impl();
    ~Impl();
    bool Connect(const int port, const char* address);
    bool Disconnect();
    void Proc(long long intervalOfTime);
    int Send(const char* buffer, const size_t bufferSize);
    int GetLastError() const;

private:
    SOCKET mSocket;
    const char* mBuffer;
    const size_t mBufferSize;
    bool mDisconnectRequest;
};

SCIClient::Impl::Impl()
    : mSocket(INVALID_SOCKET)
    , mBuffer(nullptr)
    , mBufferSize(0)
    , mDisconnectRequest(false)
{

}

SCIClient::Impl::~Impl()
{
    Disconnect();
}

bool SCIClient::Impl::Connect(const int port, const char* address)
{
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET)
    {
        ut::error("socket failure. (%d)\n", WSAGetLastError());
        return false;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // inet_addr() の警告の対処
    // https://qiita.com/parallax_kk/items/9e877542fecb4087729f
    InetPtonA(addr.sin_family, address, &addr.sin_addr.S_un.S_addr);

    bool connected = false;
    while (!connected)
    {
        if ((connected = connect(mSocket, (struct sockaddr*)&addr, sizeof(addr))) == SOCKET_ERROR)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_OF_RETRY_TIME_MILLISECONDS));

            ut::info("can not connect to server. retry.\n");
        }

        if (mDisconnectRequest)
        {
            ut::error("cancel connection.\n");

            closesocket(mSocket);
            mSocket = INVALID_SOCKET;

            return false;
        }
    }

    std::thread th(&SCIClient::Impl::Proc, this, INTERVAL_OF_PROCESS_TIME_MILLISECONDS);

    if (!th.joinable())
    {
        ut::error("thread error.\n");

        closesocket(mSocket);
        mSocket = INVALID_SOCKET;

        return false;
    }

    th.join();

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
        send(&mSocket, sys::SCIPacket::DISCONNECT);
        closesocket(mSocket);
    }
    return true;
}

int SCIClient::Impl::Send(const char* buffer, const size_t bufferSize)
{
    return ::send(mSocket, buffer, static_cast<int>(bufferSize), 0);
}

int SCIClient::Impl::GetLastError() const
{
    return WSAGetLastError();
}

void SCIClient::Impl::Proc(long long intervalOfTime)
{
    bool connected = true;
    while (connected)
    {

        char send_buffer[64] = {"goodbye"};
        if (int len = send(&mSocket, send_buffer, static_cast<int>(strlen(send_buffer))))
        {
            ut::logging("send message. %d\n", len);
        }
        else
        {
            continue;
        }

        char buffer[1024];
        if (int recvSize = recv(mSocket, buffer, sizeof(buffer), 0) > 0)
        {
            sys::SCIPacket packet;
            size_t dataSize = 0;
            packet.FromBuffer(buffer, recvSize);
            switch (packet.GetData().mHeader[sys::SCIPacket::RAWDATA_HEADER_INDEX])
            {
            case sys::SCIPacket::DISCONNECT:
                connected = false;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(intervalOfTime));
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
