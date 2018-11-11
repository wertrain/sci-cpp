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
    memcpy(buffer, &mRawData, mRawData.mHeader[RAWDATA_DATA_SIZE_INDEX]);
}

void SCIPacket::Set(const RawDataHeader header)
{
    mRawData.mHeader[0] = header;
    ::memcpy(mRawData.mHeader + sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN), RAWDATA_HEADER_MAGIC_TOKEN, sizeof(RAWDATA_HEADER_MAGIC_TOKEN));
}

bool SCIPacket::Set(const RawDataHeader header, const int8_t* body, const size_t bodySize)
{
    if (bodySize > RAWDATA_BODY_SIZE) return false;
    Set(header);
    ::memset(mRawData.mBody, 0, RAWDATA_BODY_SIZE);
    ::memcpy(mRawData.mBody, body, bodySize);
    mRawData.mHeader[RAWDATA_DATA_SIZE_INDEX] = bodySize;
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
    char buffer[sys::SCIPacket::RAWDATA_BODY_SIZE];

    sys::SCIPacket packet;
    packet.Set(header);

    sys::SCIPacket::RawData rawData = packet.GetData();
    memcpy(&rawData, buffer, sizeof(sys::SCIPacket::RawData));
    return ::send(*socket, buffer, sizeof(sys::SCIPacket::RawData), 0);
}

int SCIPacketSender::send(SOCKET* socket, const void* data, const size_t dataSize)
{
    char buffer[sys::SCIPacket::RAWDATA_BODY_SIZE];

    sys::SCIPacket packet;
    if (sys::SCIPacket::RAWDATA_BODY_SIZE >= dataSize)
    {
        packet.Set(sys::SCIPacket::MESSAGE, static_cast<const int8_t*>(data), dataSize);
        
        size_t sendDataSize;
        packet.CopyBuffer(buffer, sendDataSize);
        return ::send(*socket, buffer, sendDataSize, 0);
    }
    else
    {

    }

    sys::SCIPacket::RawData rawData = packet.GetData();
    memcpy(&rawData, buffer, sizeof(sys::SCIPacket::RawData));

}

NS_SCI_SYS_END
