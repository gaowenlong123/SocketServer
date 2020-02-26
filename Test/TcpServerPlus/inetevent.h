#ifndef INETEVENT_H
#define INETEVENT_H
#include <MessageHeader.h>

class INetEvent
{
public:

    virtual void OnNetJoin(ClientSocket* pClient)=0;

    //客户端离开事件
    virtual void OnNetLeave(ClientSocket* pClient) = 0 ;

    //客户端消息事件
    virtual void OnNetMsg(ClientSocket* pClient, DataHeader* header)=0;


    virtual void OnNetRecv(ClientSocket* pClient) = 0;
};

#endif // INETEVENT_H
