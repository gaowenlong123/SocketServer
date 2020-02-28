#include "TcpClient.h"
#include <iostream>
#include <vector>
#include "algorithm"
#include <stdio.h>
#include <thread>

using namespace std;



TcpClient::TcpClient()
{
    sendcount =0;
    m_sock =INVALID_SOCKET;
}

TcpClient::~TcpClient()
{
    Close();
}



int TcpClient::initSocket()
{

    if(m_sock != INVALID_SOCKET)
    {
        printf("Close old _sock !!\n");
        Close();
    }

#ifdef _WIN32
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);
#endif

    //建立一个socket
    m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(INVALID_SOCKET == m_sock){
        printf("_sock err!!\n");
    }

    return 0;
}

int TcpClient::Connect(char * ip , short port)
{

    if(m_sock == INVALID_SOCKET)
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

    int ret =   connect(m_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
    if(SOCKET_ERROR ==ret ){
//        printf("connect port err!!\n");
    }else {
//        printf("connect port pass!!\n");
    }

    return ret;

}


void  TcpClient::Close()
{

    if(m_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(m_sock);
        WSACleanup();
#else
        close(m_sock);
#endif

        m_sock =INVALID_SOCKET;
    }

}


bool TcpClient::OnRun(){

    if(isRun())
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(m_sock,&fdReads);
        timeval t ={5,0};

        int ret = select(m_sock+1,&fdReads ,NULL ,NULL, &t);
        if(ret < 0)
        {
            printf("select err!!\n");
            Close();
            return false;
        }

        if(FD_ISSET(m_sock,&fdReads))
        {
            FD_CLR(m_sock,&fdReads);

            if(-1 ==RecvData(m_sock))
            {
                printf(" client select quit!!\n");
                Close();
                return false;
            }
        }

        return true;

    }

    return true;

}


bool TcpClient::isRun()
{
    return m_sock != INVALID_SOCKET ;
}


int  TcpClient::RecvData(SOCKET csock){

    //5接受数据
    int nlen =  (int)recv(csock,m_szRecv,RECV_BUFF_SIZE,0);

    if(nlen<=0){
        printf("client server quit!!\n");
        return -1;
    }

    // recvbuf ==> msgbuf
    memcpy(m_szMsgBuff + m_lastMsgPos,m_szRecv,nlen);

    m_lastMsgPos+=nlen;

    //是否粘包
    while(m_lastMsgPos >= sizeof(DataHeader)){

         //是否少包
        //recv data size out of  DataHeader's size ==>  get DataHeader
        if (m_lastMsgPos  >=  sizeof(DataHeader))
        {
            DataHeader* header =(DataHeader*)m_szMsgBuff;
            if(m_lastMsgPos >= header->datalength)
            {
                int nSize =  m_lastMsgPos - header->datalength;

                OnNetMsg(header);

                //move pointer to unuse data
                memcpy(m_szMsgBuff , m_szMsgBuff + header->datalength , m_lastMsgPos - header->datalength);
                m_lastMsgPos =nSize;
            }
        }else {
            break;
        }
    }

    return 0;
}


void TcpClient::OnNetMsg(DataHeader* header)
{
    if(!header)
    {
         printf("header is empty");
    }
    // 6处理请求
    switch (header->cmd) {
    case CMD_LOGIN_RESULT:
    {
        LoginResult* login =(LoginResult*)header;

        //判断账号密码是否正确
        printf("Client recv cmd=CMD_LOGIN_RESULT ; Result=%d\n" ,login->result);

    }
        break;
    case CMD_LOGINOUT_RESULT:
    {
        //判断账号密码是否正确

        LoginOutResult* loginout =(LoginOutResult*)header;
        printf("cilent recv cmd=CMD_LOGINOUT_RESULT ; Result=%d\n" ,loginout->result);

    }
        break;

    case CMD_NEWLOGIN:
    {
        //判断账号密码是否正确

        NewLogin* newLogin =(NewLogin*)header;
        printf("cilent recv cmd=CMD_NEWLOGIN ; new SOCKID=%d\n" ,newLogin->sockId);
    }
        break;
    case CMD_Err:
    {
        printf("cilent recv cmd=CMD_Err ; datalength=%d\n" ,header->datalength);
    }
    break;
    default:
         printf("cilent recv cmd=UnDefine \n");
        break;
    }

}


int TcpClient::SendData(DataHeader* header){
    if(isRun() && header){
        //        return send(m_sock,(const char *)header,sizeof(DataHeader),0);

        int ret =send(m_sock,(const char *)header,header->datalength,0);

//        printf("send rsendsendsend ==<%d>\n",ret);

        return ret;

    }
    return SOCKET_ERROR;
}
