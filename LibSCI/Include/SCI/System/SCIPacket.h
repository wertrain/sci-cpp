/**
 * @file SCIPacket.h
 * @brief ユーティリティ
 */
#pragma once

NS_SCI_SYS_BEGIN

class SCIPacket
{
public:
    enum RawDataHeader : int8_t
    {
        READY,
        DISCONNECT
    };
    static const int8_t RAWDATA_HEADER_MAGIC_TOKEN[3];

public:
    struct RawData
    {
        int8_t mHeader[4];
        int8_t mBody[60];
        int8_t mPadding[64];
    };

public:
    explicit SCIPacket();
    ~SCIPacket();
    const RawData& GetData();

    void Set(const RawDataHeader header);
    void Set(const RawDataHeader header, const int8_t* body, const size_t bodySize);

private:
    RawData mRawData;
};

NS_SCI_SYS_END
