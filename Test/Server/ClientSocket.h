#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H
#include "MessageHeader.h"
#include <stdio.h>
#include "memory.h"
#include <memory>
#include "iostream"
#include "../Memory/CellObjPool.h"


#define CLIENT_HEART_DEAD_TIMES 5000
#define CLIENT_SEND_TIMES 200

class ClientSocket:public ObjectPoolBase<ClientSocket,1000>
{
public:

    ClientSocket(SOCKET sockfd = INVALID_SOCKET)
    {
        m_sockfd = sockfd;
        memset(m_szMsgBuff,0,sizeof(m_szMsgBuff));
        m_lastMsgPos = 0;


        memset(m_szSendBuff,0,sizeof(m_szSendBuff));
        m_lastSendPos = 0;

        resetDTSend();
        resetDTHeart();
    }

   ~ClientSocket()
   {
                printf("ClientSocket<%d>");   //???

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

    int SendData()
    {
        int ret = SOCKET_ERROR ;
        if(m_lastSendPos > 0 && SOCKET_ERROR != m_sockfd)
        {
            ret = send(m_sockfd , (const char *)m_szSendBuff , m_lastSendPos,0);

            m_lastSendPos = 0;

            resetDTSend();
        }

        return ret;

    }

    int SendData(DataHeaderPtr header)
    {
        //定量发送
        int ret = SOCKET_ERROR;

        int nSendLen = header->datalength;
        //要发送的数据
        const char* pSendData = (const char*)header.get();

        while(true)
        {
            if(m_lastSendPos + nSendLen >= SEND_BUFF_SIZE)
            {
                //计算可以copy的长度
                int  nCopyLen = SEND_BUFF_SIZE - m_lastSendPos;
                memcpy(m_szSendBuff + m_lastSendPos , pSendData , nCopyLen);

                //copy完数据进行偏移
                pSendData += nCopyLen;

                nSendLen -= nCopyLen;

                ret = send(m_sockfd , (const char *)m_szSendBuff , SEND_BUFF_SIZE,0);

//                ret = send(m_sockfd , (const char *)m_szSendBuff , SEND_BUFF_SIZE,MSG_NOSIGNAL);

                m_lastSendPos = 0;

                resetDTSend();


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

//        int ret =  send(m_sockfd,(const char *)header,header->datalength,0);
//        printf("m_lastSendPos<%d>",m_lastSendPos);
        return ret;
    }

    void resetDTHeart()
    {
        m_dtHeart =0;
    }

    void resetDTSend()
    {
        m_dtSend =0;
    }


    bool checkHeart(int dt)
    {
        m_dtHeart += dt;
        if(m_dtHeart >= CLIENT_HEART_DEAD_TIMES)
        {
            printf("checkheart dead: s=%d , time=%d\n" ,m_sockfd ,(int)m_dtHeart);
            return true;
        }

        return false;
    }

    bool checkSend(int dt)
    {
        m_dtSend += dt;
        if(m_dtSend >= CLIENT_SEND_TIMES)
        {
            printf("checkSend dead: s=%d , time=%d\n" ,m_sockfd ,(int)m_dtHeart);

            //立刻发送数据
            resetDTSend();

            SendData();
            return true;
        }

        return false;
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

    //heart dead
    int m_dtHeart;

    //上次发送数据的时间
    int m_dtSend;
};

typedef std::shared_ptr<ClientSocket> ClientSocketPtr;



#endif // CLIENTSOCKET_H
