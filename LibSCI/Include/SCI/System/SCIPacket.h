/**
 * @file SCIPacket.h
 * @brief ���[�e�B���e�B
 */
#pragma once

NS_SCI_SYS_BEGIN

class SCIPacket
{
public:
    enum RawDataHeader : int8_t
    {
        READY = 0,
        DISCONNECT
    };

    enum RawDataHeaderFlag : int8_t
    {
        FLAG_NONE    = 0 << 0,
        FLAG_CONNECT = 1 << 0,
    };

public:
    static const size_t RAWDATA_BODY_SIZE = 256;
    static const int8_t RAWDATA_HEADER_MAGIC_TOKEN[2];

    struct RawData
    {
        int8_t mHeader[4];
        int8_t mBody[RAWDATA_BODY_SIZE];
        int8_t mPadding[4];
    };

public:
    explicit SCIPacket();
    ~SCIPacket();
    const RawData& GetData();

    void Set(const RawDataHeader header);
    bool Set(const RawDataHeader header, const int8_t* body, const size_t bodySize);

private:
    RawData mRawData;
};

NS_SCI_SYS_END