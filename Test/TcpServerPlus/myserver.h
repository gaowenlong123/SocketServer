#ifndef MYSERVER_H
#define MYSERVER_H

#include "TcpServer.h"

class MyServer : public TcpServer
{
public:
    MyServer() {}

    virtual void OnNetJoin(ClientSocket* pClient)
    {
        _clentsCount++;

    }

    virtual void OnNetLeave(ClientSocket* pClient)
    {
        _clentsCount--;
    }

    virtual void OnNetRecv(ClientSocket* pClient)
    {
        _recvCount ++;
    }

    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)
    {

        _msgCount ++;

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
//                printf("Server recv cmd=CMD_LOGIN ; username= %s ; length=%d\n",login->name ,login->datalength);
//            LoginResult ret;
//            ret.result = 1;
    //        SendData(_csock,(DataHeader*) &ret);

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
