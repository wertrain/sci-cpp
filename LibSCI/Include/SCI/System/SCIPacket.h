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
        LINK_MESSAGE_BEGIN,
        LINK_MESSAGE,
        LINK_MESSAGE_END,
    };

    enum RawDataHeaderFlag : int8_t
    {
        FLAG_NONE    = 0 << 0
    };

public:
    static const size_t RAWDATA_BODY_SIZE = 1024;
    static const int RAWDATA_HEADER_INDEX = 0;
    static const int RAWDATA_FLAG_INDEX = 1;
    static const int8_t RAWDATA_HEADER_MAGIC_TOKEN[4];

    struct RawData
    {
        uint8_t  mHeader[8];
        uint16_t mDataSize;
        uint8_t  mBody[RAWDATA_BODY_SIZE];
        uint8_t  mPadding[6];
    };

public:
    explicit SCIPacket();
    ~SCIPacket();
    const RawData& GetData();
    void CopyBuffer(char* buffer, size_t& dataSize);
    bool FromBuffer(const char* buffer, const size_t dataSize);
    bool IsValid();
    void SetFlag(const int8_t flag);

    void Set(const RawDataHeader header);
    bool Set(const RawDataHeader header, const void* body, const size_t bodySize);

private:
    RawData mRawData;
};

//-------------------------------------------------------------------------------------------------

class SCIPacketSender
{
public:
    SCIPacketSender();
    virtual ~SCIPacketSender();

protected:
    int send(SOCKET* socket, const sys::SCIPacket::RawDataHeader header);
    int send(SOCKET* socket, const void* data, const size_t dataSize);
};

//-------------------------------------------------------------------------------------------------

class SCIPacketReceiver
{
public:
    SCIPacketReceiver();
    virtual ~SCIPacketReceiver();

private:
    struct LinkedData
    {
        uint8_t* mData;
        size_t mDataSize;
        LinkedData* mNextData;
        LinkedData* mPrevData;
        uint8_t mPadding[8];
    };

protected:
    bool receive(SOCKET* socket);
    void link(const uint8_t* mData, const size_t mDataSize);

private:
    LinkedData sLinkedDataPool[1024];
    LinkedData* mRootLinkedData;
};

NS_SCI_SYS_END
