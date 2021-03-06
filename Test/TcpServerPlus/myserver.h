#ifndef MYSERVER_H
#define MYSERVER_H

#include "TcpServer.h"

class MyServer : public TcpServer
{
public:
    MyServer() {}

    virtual void OnNetJoin(ClientSocket* pClient)
    {
         TcpServer::OnNetJoin(pClient);
    }

    virtual void OnNetLeave(ClientSocket* pClient)
    {
        TcpServer::OnNetLeave(pClient);
    }

    virtual void OnNetRecv(ClientSocket* pClient)
    {
        TcpServer::OnNetRecv(pClient);
    }

    virtual void OnNetMsg(CellServer* pServer,ClientSocket* pClient, DataHeader* header)
    {

        TcpServer::OnNetMsg(pServer,pClient,header);
        //处理请求
        if(!header)
        {
            printf("header is empty");
        }
        // 6处理请求
        switch (header->cmd) {
        case CMD_LOGIN:
        {
            Login* login = (Login*)header;
            //判断账号密码是否正确
           printf("Server recv cmd=CMD_LOGIN ; username= %s ; length=%d\n",login->name ,login->datalength);
            LoginResult* ret = new LoginResult();
            ret->result = 1;
            pServer->addSendTask(pClient,ret);

        }
            break;
        case CMD_LOGINOUT:
        {
            LoginOut* loginout = (LoginOut*)header;
            //判断账号密码是否正确
    //        printf("Server recv cmd=CMD_LOGINOUT ; username= %s ; length=%d\n",loginout->name ,loginout->datalength);

//            LoginOutResult outret;
//            outret.result = 1;
    //        SendData(_csock,(DataHeader*) &outret);
        }
            break;

        case CMD_Err:
        {
//            DataHeader headererr;
    //        SendData(_csock,(DataHeader*) &headererr);
        }
            break;
        default:
            printf("server recv cmd=UnDefine \n");
            break;
        }
    }
};

#endif // MYSERVER_H
