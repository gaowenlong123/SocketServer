#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include "MessageHeader.h"
class ClientSocket
{
public:

    ClientSocket(SOCKET sockfd = INVALID_SOCKET)
    {
        m_sockfd = sockfd;
        memset(m_szMsgBuff,0,sizeof(m_szMsgBuff));
        m_lastMsgPos = 0;
    }

    SOCKET* sockfd()
    {
        return &m_sockfd;

    }

    char* msgBuf()
    {
        return m_szMsgBuff;
    }

    int getlastMsgPos()
    {
        return m_lastMsgPos;
    }

    void setlastMsgPos(int pos)
    {
        m_lastMsgPos = pos;
    }


    SOCKET m_sockfd;

private:


    //msg缓冲区
    char m_szMsgBuff[RECV_BUFF_SIZE*10];

    int m_lastMsgPos;
};

#endif // CLIENTSOCKET_H
