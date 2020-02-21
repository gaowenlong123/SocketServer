#ifndef TCPSERVER_H
#define TCPSERVER_H
#ifdef _WIN32
    //windows
    #define WIN32_LEAN_AND_MEAN  //windows避免调用之前的宏定义，产生冲突
    #include <windows.h>  //windows系统头文件
    #include <winsock2.h>

#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>


    #define SOCKET  int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR           (-1)
#endif

#include "MessageHeader.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include "algorithm"

using namespace std;

class TcpServer
{

public:
    TcpServer();
    virtual ~TcpServer();

    int initSocket();

    int initBind(const char * , unsigned port);

    int initListen(int n);

    void Close();

    SOCKET socketAccept();

    bool OnRun();

    bool isRun();

    int RecvData(SOCKET _csock);

    virtual void OnNetMsg(SOCKET _csock ,DataHeader* header);

    int SendData(SOCKET _csock ,DataHeader* header);

    void SendDataToAll(DataHeader* header);


private:

    SOCKET m_sock;

    vector<SOCKET> g_clients;



private:

};

#endif // TCPSERVER_H
