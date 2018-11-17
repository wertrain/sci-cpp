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

const int8_t SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[4] = { 0xf, 0xf, 0x1, 0x2 };
static_assert(sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN) == sizeof(int8_t) * 4, "MAGIC_TOKEN");

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

void SCIPacket::CopyBuffer(char* buffer, size_t& dataSize)
{
    dataSize = sizeof(RawData);
    memcpy(buffer, &mRawData, dataSize);
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
    mRawData.mHeader[RAWDATA_DATA_SIZE_INDEX] = static_cast<uint8_t>(bodySize);
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
    char buffer[sys::SCIPacket::RAWDATA_BODY_SIZE] = { 0 };

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
        while (remainSize != 0)
        {
            size_t size = remainSize > sys::SCIPacket::RAWDATA_BODY_SIZE ? sys::SCIPacket::RAWDATA_BODY_SIZE : remainSize;
            packet.Set(sys::SCIPacket::MESSAGE, static_cast<const int8_t*>(remainData), size);

            size_t sendDataSize;
            packet.CopyBuffer(buffer, sendDataSize);
            if (int realSendSize = ::send(*socket, buffer, static_cast<int>(sendDataSize), 0))
            {
                remainSize -= realSendSize;
                remainData += realSendSize;
            }
        }
        return static_cast<int>(dataSize);
    }

}

NS_SCI_SYS_END
