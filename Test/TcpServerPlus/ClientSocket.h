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


        memset(m_szSendBuff,0,sizeof(m_szSendBuff));
        m_lastSendPos = 0;
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

    int SendData(DataHeader* header)
    {
        int ret = SOCKET_ERROR;

        int nSendLen = header->datalength;
        //要发送的数据
        const char* pSendData = (const char*)header;

        while(true)
        {
            if(m_lastSendPos + nSendLen >= SEND_BUFF_SIZE)
            {
                //计算可以copy的长度
                int  nCopyLen = SEND_BUFF_SIZE - m_lastSendPos;
                memcpy(m_szSendBuff + m_lastSendPos , pSendData , nCopyLen);

                //copy完数据进行偏移
                pSendData += nCopyLen;

                nSendLen -= nSendLen;

                ret = send(m_sockfd , m_szSendBuff , SEND_BUFF_SIZE,0);

                m_lastSendPos = 0;

                if(ret == SOCKET_ERROR)
                {
                    return ret;
                }

            }
            else{
                memcpy(m_szSendBuff + m_lastSendPos , pSendData , nSendLen);
                m_lastSendPos += nSendLen;
                break;
            }
        }


        return ret;
    }


    SOCKET m_sockfd;

private:


    //第二缓冲区  消息缓冲区
    char m_szMsgBuff[RECV_BUFF_SIZE];

    //消息缓冲区的数据尾部位置
    int m_lastMsgPos;

    //发送缓冲区
    char m_szSendBuff[SEND_BUFF_SIZE];

    //消息缓冲区的数据尾部位置
    int m_lastSendPos;
};

#endif // CLIENTSOCKET_H
