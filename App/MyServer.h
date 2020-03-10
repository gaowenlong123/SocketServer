#ifndef MYSERVER_H
#define MYSERVER_H

#include "TcpServer.h"
#include "../../Public/Stream/CBaseStream.h"

class MyServer : public TcpServer
{
public:
    MyServer() {}

    virtual void OnNetJoin(ClientSocketPtr& pClient)
    {
         TcpServer::OnNetJoin(pClient);
    }

    virtual void OnNetLeave(ClientSocketPtr& pClient)
    {
        TcpServer::OnNetLeave(pClient);
    }

    virtual void OnNetRecv(ClientSocketPtr& pClient)
    {
        TcpServer::OnNetRecv(pClient);
    }

    virtual void OnNetMsg(CellServer* pServer,ClientSocketPtr& pClient, DataHeader* header)
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
//            LoginResult* ret = new LoginResult();
            LoginResultPtr ret = std::make_shared<LoginResult>();
            ret->result = 1;
            pServer->addSendTask(pClient,(DataHeaderPtrRef)ret);

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

        case CMD_HEART:
        {

            Heart* heart = (Heart*)header;
            printf("Server recv cmd=CMD_HEART ; result= %d ; length=%d\n",heart->result);

            pClient->resetDTHeart();

            HeartPtr ret = std::make_shared<Heart>();
            ret->result = 1;
            pServer->addSendTask(pClient,(DataHeaderPtrRef)ret);

        }

            break;

        default:
            printf("server recv cmd=UnDefine \n");
            break;
        }
    }
};

#endif // MYSERVER_H
