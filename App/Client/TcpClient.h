#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "../../Public/Define/MessageDefine.h"
#include "../TcpInterface/ClientSocket.h"
#include "../../Public/Log/CBaseLog.h"
#include <atomic>


class TcpClient
{
public:
    TcpClient();
    virtual ~TcpClient();

    int initSocket();

    int Connect(char * ip , unsigned short port);

    void Close();

    bool OnRun();

    bool isRun();

    int RecvData();



    int SendData(DataHeader* header,int nLen);

protected:
     virtual void OnNetMsg(DataHeader* header)=0;
    ClientSocket* m_Clientsock = nullptr;

private:

    bool m_isConnect;


    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //msg缓冲区
    char m_szMsgBuff[RECV_BUFF_SIZE*10]={};

    int m_lastMsgPos = 0;


    std::atomic_int sendcount;
};




#endif // TCPCLIENT_H
