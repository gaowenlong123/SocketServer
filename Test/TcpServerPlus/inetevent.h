#ifndef INETEVENT_H
#define INETEVENT_H
#include <MessageHeader.h>

class INetEvent
{
public:

    //客户端离开事件
    virtual void OnLeave(ClientSocket* pClient) = 0 ;

    //客户端消息事件
    virtual void OnNetMsg(SOCKET cSock, DataHeader* header)=0;
};

#endif // INETEVENT_H
