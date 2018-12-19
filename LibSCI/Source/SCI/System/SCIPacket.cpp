/**
 * @file SCIUtility.cpp
 * @brief ユーティリティ
 */
#include <Precompiled.h>
#include <memory>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SCI/System/SCIPacket.h>

NS_SCI_SYS_BEGIN

const int8_t SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[4] = { 0x3, 0xc, 0x1, 0xf };
static_assert(sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN) == sizeof(int8_t) * 4, "sizeof MAGIC_TOKEN");

SCIPacket::SCIPacket()
    : mRawData()
{
    ::memset(&mRawData, 0, sizeof(mRawData));
}

SCIPacket::~SCIPacket()
{

}

const SCIPacket::RawData& SCIPacket::GetData()
{
    return mRawData;
}

const SCIPacket::RawData* SCIPacket::GetDataPointer()
{
    return &mRawData;
}

void SCIPacket::CopyBuffer(char* buffer, size_t& dataSize)
{
    dataSize = sizeof(RawData);
    memcpy(buffer, &mRawData, dataSize);
}

bool SCIPacket::FromBuffer(const char* buffer, const size_t dataSize)
{
    if (sizeof(RawData) < dataSize) return false;
    ::memcpy(mRawData.mHeader, buffer, dataSize);
    return true;
}

bool SCIPacket::IsValid()
{
    uint8_t* p = mRawData.mHeader + sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN);
    if (p[0] == SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[0] &&
        p[1] == SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[1] &&
        p[2] == SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[2] &&
        p[3] == SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[3])
    {
        return true;
    }
    return false;
}

void SCIPacket::SetFlag(const int8_t flag)
{
    mRawData.mHeader[RAWDATA_FLAG_INDEX] = flag;
}

void SCIPacket::Set(const RawDataHeader header)
{
    mRawData.mHeader[0] = header;
    ::memcpy(mRawData.mHeader + sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN), RAWDATA_HEADER_MAGIC_TOKEN, sizeof(RAWDATA_HEADER_MAGIC_TOKEN));
}

bool SCIPacket::Set(const RawDataHeader header, const void* body, const size_t bodySize)
{
    if (bodySize > RAWDATA_BODY_SIZE) return false;
    Set(header);
    ::memset(mRawData.mBody, 0, RAWDATA_BODY_SIZE);
    ::memcpy(mRawData.mBody, body, bodySize);
    mRawData.mDataSize = static_cast<uint32_t>(bodySize);
    return true;
}

//-------------------------------------------------------------------------------------------------

SCIPacketSender::SCIPacketSender()
{

}

SCIPacketSender::~SCIPacketSender()
{

}

int SCIPacketSender::send(SOCKET* socket, const sys::SCIPacket::RawDataHeader header)
{
    size_t dataSize = 0;
    char buffer[sizeof(sys::SCIPacket)] = { 0 };

    sys::SCIPacket packet;
    packet.Set(header);
    packet.CopyBuffer(buffer, dataSize);

    return ::send(*socket, buffer, static_cast<int>(dataSize), 0);
}

int SCIPacketSender::send(SOCKET* socket, const void* data, const size_t dataSize)
{
    char buffer[sizeof(SCIPacket::RawData)];

    sys::SCIPacket packet;
    if (sys::SCIPacket::RAWDATA_BODY_SIZE >= dataSize)
    {
        packet.Set(sys::SCIPacket::MESSAGE, static_cast<const int8_t*>(data), dataSize);
        
        size_t sendDataSize;
        packet.CopyBuffer(buffer, sendDataSize);
        return ::send(*socket, buffer, static_cast<int>(sendDataSize), 0);
    }
    else
    {
        const int8_t* remainData = static_cast<const int8_t*>(data);
        size_t remainSize = dataSize;

        size_t size = sys::SCIPacket::RAWDATA_BODY_SIZE;
        packet.Set(sys::SCIPacket::LINK_MESSAGE_BEGIN, static_cast<const int8_t*>(remainData), size);

        while (remainSize != 0)
        {
            size_t sendDataSize;
            packet.CopyBuffer(buffer, sendDataSize);
            if (int realSendSize = ::send(*socket, buffer, static_cast<int>(sendDataSize), 0))
            {
                remainSize -= realSendSize;
                remainData += realSendSize;
            }

            if (remainSize == 0)
            {
                break;
            }
            else if (remainSize > sys::SCIPacket::RAWDATA_BODY_SIZE)
            {
                size = sys::SCIPacket::RAWDATA_BODY_SIZE;
                packet.Set(sys::SCIPacket::LINK_MESSAGE, static_cast<const int8_t*>(remainData), size);
            }
            else
            {
                size = remainSize;
                packet.Set(sys::SCIPacket::LINK_MESSAGE_END, static_cast<const int8_t*>(remainData), size);
            }
        }
        return static_cast<int>(dataSize);
    }

}

//-------------------------------------------------------------------------------------------------

SCIPacketReceiver::SCIPacketReceiver()
    : mRootLinkedData(nullptr)
{

}

SCIPacketReceiver::~SCIPacketReceiver()
{

}

bool SCIPacketReceiver::receive(SOCKET* socket)
{
    char buffer[sys::SCIPacket::RAWDATA_BODY_SIZE];
    int recvSize = 0;
    if ((recvSize = recv(*socket, buffer, sizeof(buffer), 0)) > 0)
    {
        sys::SCIPacket packet;
        size_t dataSize = 0;
        packet.FromBuffer(buffer, recvSize);
        switch (packet.GetData().mHeader[sys::SCIPacket::RAWDATA_HEADER_INDEX])
        {
            case sys::SCIPacket::MESSAGE:
            {
                uint8_t* data = new uint8_t[packet.GetData().mDataSize];
                memcpy(data, packet.GetDataPointer(), packet.GetData().mDataSize);
                link(data, packet.GetData().mDataSize);
                break;
            }
        }
    }
    return true;
}

bool SCIPacketReceiver::link(const uint8_t* data, const size_t dataSize)
{
    LinkedData* target = nullptr;
    for (int i = 0; i < LINKED_DATA_POOL_SIZE; ++i)
    {
        if (mLinkedDataPool[i].mData == nullptr)
        {
            target = &mLinkedDataPool[i];
            break;
        }
    }
    if (target == nullptr) return false;

    memset(target, 0, sizeof(LinkedData));

    auto* p = mRootLinkedData;
    while (p)
    {
        if (p->mNextData == nullptr)
        {
            p->mNextData = target;
            target->mPrevData = p;
            break;
        }
        p = p->mNextData;
    }
    return true;
}

NS_SCI_SYS_END
