#ifndef CELLSERVER_H
#define CELLSERVER_H
#include "ClientSocket.h"
#include "inetevent.h"



#include <mutex>
#include <thread>
#include <vector>
#include <atomic>


using namespace std;

class CellServer
{

public:
    CellServer(){}
    CellServer(SOCKET sock = INVALID_SOCKET);

    ~CellServer();

    void Start();

    //只用来读取客户端发来的数据
    bool OnRun();

    bool isRun();

    int RecvData(ClientSocket* pclient);

    void OnNetMsg(SOCKET _csock ,DataHeader* header);

    int SendData(SOCKET _csock ,DataHeader* header);

    void SendDataToAll(DataHeader* header);

    void Close();

    void addClient(ClientSocket* client);

    size_t getClientSize();

    void setEventOj(INetEvent* event);


private:
    SOCKET m_sock;

    //正式的客户
    vector<ClientSocket*> m_clients;

    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //缓冲队列里的客户
    vector<ClientSocket*> m_clientsBuff;

    std::mutex m_mutex;

    std::thread m_pthread;

    INetEvent* m_pNetEvent;

};

#endif // CELLSERVER_H
