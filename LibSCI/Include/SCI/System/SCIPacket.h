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
    static const int RAWDATA_HEADER_INDEX = 0;
    static const int RAWDATA_FLAG_INDEX = 1;
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

class SCIPacketSender
{
public:
    SCIPacketSender();
    virtual ~SCIPacketSender();

protected:
    void send(SOCKET* socket, const sys::SCIPacket::RawDataHeader header);
};

NS_SCI_SYS_END
