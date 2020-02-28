#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <thread>
#include <iostream>
#include <vector>
#include <stdio.h>
#include "algorithm"

#include "MessageHeader.h"
#include "ClientSocket.h"
#include "celltimestamp.h"
#include "CellServer.h"
#include "inetevent.h"
#include "iostream"



#define _CELL_THREAD_COUNT 1

using namespace std;

class TcpServer : public INetEvent
{

public:
    TcpServer();
    virtual ~TcpServer();

    int initSocket();

    int initBind(const char * , unsigned port);

    int initListen(int n);


    SOCKET socketAccept();

    void addClientToCellServer(ClientSocket* pClient);

    void startServerThread();


    void timeForMsg();


    bool OnRun();

    bool isRun();


    void Close();

    virtual void OnNetLeave(ClientSocket* pClient)
    {
        _clentsCount--;
    }


    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
    {
        _msgCount ++;
    }

    virtual void OnNetJoin(ClientSocket* pClient)
    {
        _clentsCount++;
    }

    virtual void OnNetRecv(ClientSocket* pClient)
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

    vector<ClientSocket*> m_clients;

    vector<CellServer*> m_cellServers;

    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //计时器
    CELLTimestamp m_tTime;


    //msg缓冲区
    //    char m_szMsgBuff[RECV_BUFF_SIZE*10]={};

    //    int m_lastMsgPos = 0;

};

#endif // TCPSERVER_H
