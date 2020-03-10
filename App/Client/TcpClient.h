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
private:

    bool m_isConnect;

    ClientSocket* m_Clientsock = nullptr;

    //缓冲区
    char m_szRecv[RECV_BUFF_SIZE]={};

    //msg缓冲区
    char m_szMsgBuff[RECV_BUFF_SIZE*10]={};

    int m_lastMsgPos = 0;


    std::atomic_int sendcount;
};


class MyTcpClient : public TcpClient
{
    virtual void OnNetMsg(DataHeader* header)
   {
       if(!header)
       {
            CBaseLog::Info("header is empty");
       }
       // 6处理请求
       switch (header->cmd) {
       case CMD_LOGIN_RESULT:
       {
           LoginResult* login =(LoginResult*)header;

           //判断账号密码是否正确
           CBaseLog::Info("Client recv cmd=CMD_LOGIN_RESULT ; Result=%d\n" ,login->result);

       }
           break;
       case CMD_LOGINOUT_RESULT:
       {
           //判断账号密码是否正确

           LoginOutResult* loginout =(LoginOutResult*)header;
           CBaseLog::Info("cilent recv cmd=CMD_LOGINOUT_RESULT ; Result=%d\n" ,loginout->result);

       }
           break;

       case CMD_NEWLOGIN:
       {
           //判断账号密码是否正确

           NewLogin* newLogin =(NewLogin*)header;
           CBaseLog::Info("cilent recv cmd=CMD_NEWLOGIN ; new SOCKID=%d\n" ,newLogin->sockId);
       }
           break;
       case CMD_Err:
       {
           CBaseLog::Info("cilent recv cmd=CMD_Err ; datalength=%d\n" ,header->datalength);
       }
       break;
       default:
            CBaseLog::Info("cilent recv cmd=UnDefine \n");
           break;
       }
}
};


#endif // TCPCLIENT_H
