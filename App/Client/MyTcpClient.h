#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H
#include "TcpClient.h"
#include "../../Public/Stream/CMsgStream.h"


class MyTcpClient : public TcpClient
{

public:

    MyTcpClient()
    {
    }

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
            //字节流读数据
            CRecvMsgStream r(header);
            //预读取长度
            r.readInt16();
            uint16_t cmd;
            r.Read<uint16_t>(cmd);
            auto n1 = r.readInt8();

            char name[32] = {};
            auto n2 = r.ReadArray(name ,32);


        }
            break;

        case CMD_NEWLOGIN:
        {
            //字节流写数据
             CSendMsgStream s;
             s.Write<uint16_t>(CMD_NEWLOGIN);
             s.writeInt32(this->m_Clientsock->m_sockfd);

//             char* str = "gao";
//             s.writeArray(str,strlen(str));

//             char a[] = "abcd";
//             s.writeArray(a,strlen(a));

//             int b[] = {1,2,3,4,5};
//             s.writeArray(b,5);
             s.finsh();
            this->m_Clientsock->SendData((DataHeader*)s.Data());


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



#endif // MYTCPCLIENT_H
