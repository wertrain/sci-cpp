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
        DISCONNECT,
        MESSAGE,
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
    static const int RAWDATA_DATA_SIZE_INDEX = 2;
    static const int8_t RAWDATA_HEADER_MAGIC_TOKEN[4];

    struct RawData
    {
        uint8_t mHeader[8];
        uint8_t mBody[RAWDATA_BODY_SIZE];
        uint8_t mPadding[8];
    };

public:
    explicit SCIPacket();
    ~SCIPacket();
    const RawData& GetData();
    void CopyBuffer(char* buffer, size_t& dataSize);
    bool FromBuffer(const char* buffer, const size_t dataSize);
    bool IsValid();

    void Set(const RawDataHeader header);
    bool Set(const RawDataHeader header, const void* body, const size_t bodySize);

private:
    RawData mRawData;
};

class SCIPacketSender
{
public:
    SCIPacketSender();
    virtual ~SCIPacketSender();

protected:
    int send(SOCKET* socket, const sys::SCIPacket::RawDataHeader header);
    int send(SOCKET* socket, const void* data, const size_t dataSize);
};

NS_SCI_SYS_END
