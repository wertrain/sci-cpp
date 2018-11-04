/**
 * @file SCIServer.cpp
 * @brief サーバを表すクラス
 */
#include <Precompiled.h>

#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <algorithm>
#include <vector>
#include <SCI/System/SCIPacket.h>
#include <SCI/System/SCIUtility.h>
#include <SCI/Server/SCIServer.h>

namespace sci
{

/// クライアント最大接続数
static const size_t MAX_CLIENT_NUM = 8;
/// スレッド待機時間
static const long long INTERVAL_OF_TIME_MILLISECONDS = 1000;

/// プロセスを表すクラス
struct Process
{
public:
    Process::Process(const long long intervalTime)
        : mThread()
        , mIntervalTime(intervalTime)
    {

    }
    void SetThread(std::thread* thread) { mThread = thread; }
    std::thread* GetThread() { return mThread; }
    long long GetIntervalTime() { return mIntervalTime; }

private:
    std::thread* mThread;
    long long mIntervalTime;
};

/// サーバ実装クラス
class SCIServer::Impl
{
public:
    Impl();
    ~Impl();
    bool Connect(const int port, const char* address);
    bool Disconnect();
    void Proc(Process* process);

private:
    bool createNewProcess();

private:
    SOCKET mSocket;
    std::vector<Process*> mProcessList;
};

SCIServer::Impl::Impl()
    : mSocket(INVALID_SOCKET)
    , mProcessList()
{

}

SCIServer::Impl::~Impl()
{
    Disconnect();
}

bool SCIServer::Impl::createNewProcess()
{
    Process* process = new Process(INTERVAL_OF_TIME_MILLISECONDS);
    auto thread = new std::thread(&SCIServer::Impl::Proc, this, process);
    process->SetThread(thread);

    if (!thread->joinable())
    {
        delete thread;
        delete process;
        return false;
    }

    thread->join();

    mProcessList.push_back(process);

    return true;
}

bool SCIServer::Impl::Connect(const int port, const char* address)
{
    mSocket = socket(PF_INET, SOCK_STREAM, 0);
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

    if (int error = bind(mSocket, (struct sockaddr *)&addr, sizeof(addr)))
    {
        ut::error("socket bind error. (%d)\n", WSAGetLastError());
        return false;
    }

    const int backlog = 1;
    if (int error = listen(mSocket, backlog))
    {
        ut::error("socket listen error. (%d)\n", WSAGetLastError());
        return false;
    }

    // 最初のプロセスを作成
    createNewProcess();

    return true;
}

bool SCIServer::Impl::Disconnect()
{
    if (mSocket == INVALID_SOCKET)
    {
        return false;
    }
    else
    {
        closesocket(mSocket);
    }

    for each(auto process in mProcessList)
    {
        std::thread* thread = process->GetThread();
        delete thread;
        delete process;
    }
    mProcessList.clear();

    return true;
}

void SCIServer::Impl::Proc(Process* process)
{
    // 接続を待機する
    struct sockaddr_in addr = { 0 };
    int len = sizeof(addr);

    ut::logging("wait connection. please start client.\n");
    SOCKET sockclient = accept(mSocket, (struct sockaddr *)&addr, &len);

    // 次の接続待ちを開始
    createNewProcess();

    // 受信ループ
    bool connected = true;
    while (connected)
    {
        char buffer[1024];
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
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(process->GetIntervalTime()));
    }

    closesocket(sockclient);

    ut::logging("disconnected client.\n");
}

//-------------------------------------------------------------------------------------------------

SCIServer::SCIServer()
    : mImpl(new SCIServer::Impl)
{

}

SCIServer::~SCIServer()
{

}

bool SCIServer::Start(const int port, const char* address)
{
    return mImpl->Connect(port, address);
}

bool SCIServer::End()
{
    return mImpl->Disconnect();
}

}; // namespace sci
