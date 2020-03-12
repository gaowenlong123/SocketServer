#ifndef CMSGSTREAM_H
#define CMSGSTREAM_H
#include "CBaseStream.h"
#include "../Define/MessageDefine.h"

class CRecvMsgStream : public CBaseStream
{
public:
    CRecvMsgStream(DataHeader* header)
     :CBaseStream((char*)header,header->datalength)
    {
        //先把长度占住，因为不知道消息流是否接受完
        push(header->datalength);

    }

    ~CRecvMsgStream()
    {
    }

};



class CSendMsgStream : public CBaseStream
{
public:

    CSendMsgStream(int nSize = 1024)
        :CBaseStream(nSize)
    {
        //预先占消息头所需要的空间
        Write<uint16_t>(0);

    }

    CSendMsgStream(char* pData , int nSize ,bool bDelete = false)
        :CBaseStream(pData,nSize,bDelete)
    {
        //预先占消息头所需要的空间
        Write<uint16_t>(0);
    }

    ~CSendMsgStream()
    {
    }


    void finsh()
    {
        int pos = this->length();
        this->SetWritePos(0);
        Write<uint16_t>(pos);
        SetWritePos(pos );
    }


};

#endif // CMSGSTREAM_H
