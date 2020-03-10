#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <thread>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "algorithm"
#include "memory.h"

#include "../../Public/Define/MessageDefine.h"
#include "../TcpInterface/ClientSocket.h"
#include "../TcpInterface/CellServer.h"
#include "../../Public/Time/CBaseTime.h"
#include "iostream"
#include "../../Public/Thread/CBaseThread.h"


#define _CELL_THREAD_COUNT 4


using namespace std;


class TcpServer : public CBaseEvent
{

public:
    TcpServer();
    virtual ~TcpServer();

    int initSocket();

    int initBind(const char * , unsigned port);

    int initListen(int n);


    SOCKET socketAccept();

    void addClientToCellServer(ClientSocketPtr& pClient);

    void startServerThread();


    void timeForMsg();


    bool OnRun(CBaseThread* pThread);


    void Close();

    virtual void OnNetLeave(ClientSocketPtr& pClient)
    {
        _clentsCount--;
    }


    virtual void OnNetMsg(CellServer* pServer, ClientSocketPtr& pClient, DataHeader* header)
    {
        _msgCount ++;
    }

    virtual void OnNetJoin(ClientSocketPtr& pClient)
    {
        _clentsCount++;
    }

    virtual void OnNetRecv(ClientSocketPtr& pClient)
    {
        _recvCount ++;
    }

    //    int RecvData(ClientSocket* _csock);

    //    int SendData(SOCKET _csock ,DataHeader* header);



    //计数
    std::atomic_int _recvCount;
    std::atomic_int _clentsCount;
    std::atomic_int _msgCount;


private:

    SOCKET m_sock;

    vector<ClientSocketPtr> m_clients;

    vector<CellServerPtr> m_cellServers;

    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //计时器
    CBaseTime m_tTime;


    CBaseThread m_pthread;

};

#endif // TCPSERVER_H
