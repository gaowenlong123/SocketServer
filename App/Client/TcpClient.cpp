#include "TcpClient.h"
#include <iostream>
#include <vector>
#include "algorithm"
#include <stdio.h>
#include <thread>
#include "../../Public/Log/CBaseLog.h"

using namespace std;



TcpClient::TcpClient()
{
    m_isConnect = false;
    sendcount =0;
}

TcpClient::~TcpClient()
{
    Close();
}



int TcpClient::initSocket()
{

    if(!m_Clientsock)
    {
        CBaseLog::Info("Close old _sock !!\n");
        Close();
    }

#ifdef _WIN32
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);
#else

//    忽略异常信号，防止进程被终止
//    signal(SIGPIPE ,SIG_IGN);

#endif

    //建立一个socket
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(INVALID_SOCKET == _sock){
        CBaseLog::Info("_sock err!!\n");
    }else
    {
          m_Clientsock  = new ClientSocket(_sock);
    }

    return 0;
}

int TcpClient::Connect(char * ip , unsigned short port)
{

    if(!m_Clientsock)
    {
        initSocket();
    }

    //connect 连接服务器
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;

#ifdef _WIN32
    //我们自己的主机不止一个IP地址
    _sin.sin_addr.S_un.S_addr= inet_addr(ip);   // INADDE_ANY 本机任何的地址都可以访问
#else
    _sin.sin_addr.s_addr= inet_addr(ip );
#endif
    //主机的short 转换到网络中的类型
    _sin.sin_port=htons(port);

    int ret =   connect(m_Clientsock->m_sockfd,(sockaddr*)&_sin,sizeof(sockaddr_in));
    if(SOCKET_ERROR ==ret ){
        CBaseLog::Info("connect port err!!\n");
    }else {
        CBaseLog::Info("connect port pass!!\n");
        m_isConnect = true;
    }

    return ret;

}


void  TcpClient::Close()
{

    if(m_Clientsock)
    {
        delete m_Clientsock;

        m_isConnect = false;

//#ifdef _WIN32
//        closesocket(m_sock);
//        WSACleanup();
//#else
//        close(m_sock);
//#endif

    }

}


bool TcpClient::OnRun(){

    if(isRun())
    {
        SOCKET m_sock = m_Clientsock->m_sockfd;

        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(m_sock,&fdReads);
        fd_set fdWrite;
        FD_ZERO(&fdWrite);


        timeval t ={0,2};
        int ret;
        //需要写的时候，就写
        if(m_Clientsock->getlastSendMsgPos() > 0)
        {
            FD_SET(m_sock,&fdWrite);
            ret = select(m_sock+1,&fdReads ,&fdWrite ,nullptr, &t);

        }else
        {
           ret = select(m_sock+1,&fdReads ,nullptr ,nullptr, &t);
        }

        if(ret < 0)
        {
            CBaseLog::Info("select err!!\n");
            Close();
            return false;
        }

        if(FD_ISSET(m_sock,&fdReads))
        {
            if(-1 ==RecvData())
            {
                CBaseLog::Info(" client select quit!!\n");
                Close();
                return false;
            }
        }

        //客户端写的通道大部分是空闲的 ,实现异步读写
        if(FD_ISSET(m_sock,&fdWrite))
        {
            if(-1 ==m_Clientsock->SendData())
            {
                CBaseLog::Info(" client send err!!\n");
            }
        }

        return true;

    }

    return false;

}

bool TcpClient::isRun()
{
    return m_isConnect ;
}


int  TcpClient::RecvData(){

//    char* szRecv = m_szMsgBuff + m_lastMsgPos;
//    int nLen = (int)recv(csock, szRecv, (RECV_BUFF_SIZE) - m_lastMsgPos, 0);
//    //CBaseLog::Info("nLen=%d\n", nLen);
//    if (nLen <= 0)
//    {
//        printf("client server quit!!\n");
//        return -1;
//    }

//    m_lastMsgPos += nLen;

//    while (m_lastMsgPos >= sizeof(DataHeader))
//    {

//        DataHeader* header = (DataHeader*)m_szMsgBuff;

//        if (m_lastMsgPos >= header->datalength)
//        {
//            int nSize = m_lastMsgPos - header->datalength;

//            OnNetMsg(header);

//            memcpy(m_szMsgBuff, m_szMsgBuff + header->datalength, nSize);

//            m_lastMsgPos = nSize;
//        }
//        else {
//            break;
//        }
//    }
//    return 0;



    char* szRecv = m_Clientsock->msgBuf() + m_Clientsock->getlastMsgPos();
    int nLen = (int)recv(m_Clientsock->m_sockfd, szRecv, (RECV_BUFF_SIZE)- m_Clientsock->getlastMsgPos(), 0);


    if (nLen <= 0)
    {
        printf("client <%d> quit \n",m_Clientsock->m_sockfd);
        return -1;
    }


    m_Clientsock->setlastMsgPos(m_Clientsock->getlastMsgPos() + nLen);


    while (m_Clientsock->getlastMsgPos() >= sizeof(DataHeader))
    {
        DataHeader* header = (DataHeader*)m_Clientsock->msgBuf();

        if (m_Clientsock->getlastMsgPos() >= header->datalength)
        {

            int nSize = m_Clientsock->getlastMsgPos() - header->datalength;

            OnNetMsg(header);

            memcpy(m_Clientsock->msgBuf(), m_Clientsock->msgBuf() + header->datalength, nSize);

            m_Clientsock->setlastMsgPos(nSize);
        }
        else {

            break;
        }
    }
}

int TcpClient::SendData(DataHeader* header,int nLen)
{
    if(isRun())
    {
     return m_Clientsock->SendData(header);
    }
    return 0;
}








