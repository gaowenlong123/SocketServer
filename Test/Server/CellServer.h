#ifndef CELLSERVER_H
#define CELLSERVER_H
#include "iostream"
#include "ClientSocket.h"
#include "CellTask.h"
#include "INetEvent.h"
#include "CellTimestamp.h"
#include "CELLThread.h"

#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include "memory.h"


using namespace std;

/*
class CellSendMag2ClientTask : public CellTask
{
    ClientSocketPtr m_pClient;
    DataHeaderPtr m_header;

public:
    CellSendMag2ClientTask(ClientSocketPtr pClient, DataHeaderPtr header)
    {
        m_pClient = pClient;
        m_header = header;
    }

    virtual void doTask()
    {
        int ret = m_pClient->SendData(m_header);
         printf("aaaaaaaaaaaaaaaaaaaaa===<%d>\n",ret);
//        delete m_header;
    }
};

typedef std::shared_ptr<CellSendMag2ClientTask> CellSendMag2ClientTaskPtr;

*/



typedef std::shared_ptr<CellServer> CellServerPtr;

class CellServer
{

public:
    CellServer(){}
    CellServer(SOCKET sock = INVALID_SOCKET);

    ~CellServer();

    void Start();

    //只用来读取客户端发来的数据
    bool OnRun(CELLThread* pThread);

    bool isRun();

    int RecvData(ClientSocketPtr pclient);

    void OnNetMsg(ClientSocketPtr pClient, DataHeader* header);

    void Close();

    void addClient(ClientSocketPtr client);

    size_t getClientSize();

    void setEventOj(INetEvent* event);

    //?????
    int SendData(SOCKET _csock ,DataHeader* header);

    void SendDataToAll(DataHeader* header);

    void addSendTask(ClientSocketPtr& pClient, DataHeaderPtr& header);

    void ClearClients(){}


    bool  WriteData(fd_set& _fd);

private:
    SOCKET m_sock;

    //正式的客户
    vector<ClientSocketPtr> m_clients;

    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //缓冲队列里的客户
    vector<ClientSocketPtr> m_clientsBuff;

    std::mutex m_mutex;

    CELLThread m_pthread;

    INetEvent* m_pNetEvent;

    std::atomic_uint  count ;

    CellTaskServer m_TaskServer;

    time_t _oldTime ;



};




#endif // CELLSERVER_H
