#ifndef INETEVENT_H
#define INETEVENT_H
#include <MessageHeader.h>
#include <CellServer.h>


class CellServer;
class INetEvent
{
public:

    virtual void OnNetJoin(ClientSocketPtr& pClient)=0;

    //客户端离开事件
    virtual void OnNetLeave(ClientSocketPtr& pClient) = 0 ;

    //客户端消息事件
    virtual void OnNetMsg(CellServer* pServer, ClientSocketPtr& pClient, DataHeader* header)=0;


    virtual void OnNetRecv(ClientSocketPtr& pClient) = 0;
};

#endif // INETEVENT_H
