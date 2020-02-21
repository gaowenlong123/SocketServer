#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#ifdef _WIN32
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

class TcpClient
{


public:
    TcpClient();
    virtual ~TcpClient();

    int initSocket();

    int Connect(char * ip , short port);

    void Close();

    bool OnRun();

    bool isRun();

    int RecvData(SOCKET _csock);

    void OnNetMsg(DataHeader* header);

    int SendData(DataHeader* header);
private:

    SOCKET m_sock;

};



#endif // TCPCLIENT_H
