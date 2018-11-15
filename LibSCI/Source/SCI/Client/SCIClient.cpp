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

static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

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
};

SCIClient::Impl::Impl()
    : mSocket(INVALID_SOCKET)
    , mBuffer(nullptr)
    , mBufferSize(0)
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

    if (connect(mSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        ut::error("socket connect error. (%d)\n", WSAGetLastError());
        return false;
    }

    std::thread th(&SCIClient::Impl::Proc, this, INTERVAL_OF_TIME_MILLISECONDS);

    if (!th.joinable())
    {
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
    int count = 0;
    bool connected = true;
    while (connected)
    {
        ++count;
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalOfTime));
        ut::logging("%d\n", count);

        char send_buffer[64] = {"hello"};
        sys::SCIPacket packet;
        packet.Set(sys::SCIPacket::MESSAGE, send_buffer, static_cast<int>(strlen(send_buffer)));

        char buffer[1024];
        size_t size = 0;
        packet.CopyBuffer(buffer, size);
        if (int len = send(&mSocket, buffer, size) > 0)
        {
            ut::logging("%d\n", len);
        }
        else
        {
            continue;
        }

        /*char buffer[1024];
        if (recv(mSocket, buffer, sizeof(buffer), 0) > 0)
        {
            sys::SCIPacket::RawData rawData;
            memcpy(&rawData, buffer, sizeof(sys::SCIPacket::RawData));
            switch (rawData.mHeader[sys::SCIPacket::RAWDATA_HEADER_INDEX])
            {
            case sys::SCIPacket::DISCONNECT:
                connected = false;
                break;
            }
        }*/
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
