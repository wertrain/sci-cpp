/**
 * @file SCIUtility.cpp
 * @brief ユーティリティ
 */
#include <Precompiled.h>
#include <memory>
#include <SCI/System/SCIPacket.h>

NS_SCI_SYS_BEGIN

const int8_t SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN[2] = { 0xf, 0xe };
static_assert(sizeof(SCIPacket::RAWDATA_HEADER_MAGIC_TOKEN) == sizeof(int8_t) * 2, "MAGIC_TOKEN");

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

void SCIPacket::Set(const RawDataHeader header)
{
    mRawData.mHeader[0] = header;
    ::memcpy(&mRawData.mHeader[2], RAWDATA_HEADER_MAGIC_TOKEN, sizeof(RAWDATA_HEADER_MAGIC_TOKEN));
}

bool SCIPacket::Set(const RawDataHeader header, const int8_t* body, const size_t bodySize)
{
    if (bodySize > RAWDATA_BODY_SIZE) return false;
    Set(header);
    ::memcpy(mRawData.mBody, 0, RAWDATA_BODY_SIZE);
    ::memcpy(mRawData.mBody, body, bodySize);
    return true;
}

NS_SCI_SYS_END
