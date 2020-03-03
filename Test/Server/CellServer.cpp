#include "CellServer.h"
#include "iostream"
CellServer::CellServer(SOCKET sock)
{
    m_sock = sock;
    m_pNetEvent = nullptr;
    count = 1;

    _oldTime = CELLTime::getNowInMilliSec();
}


CellServer::~CellServer()
{
    delete m_pNetEvent;
    Close();
    m_sock = INVALID_SOCKET;
}

void CellServer::Start()
{
    m_pthread = std::thread(std::mem_fn(&CellServer::OnRun) ,this);
    m_pthread.detach();

    m_TaskServer.StartTask();
}


//只用来读取客户端发来的数据
bool CellServer::OnRun()
{
    fd_set _fdead_back;
    bool _clients_change;
    SOCKET maxSock ;

    _clients_change = true;

    while(isRun())
    {
        if(m_clientsBuff.size() > 0)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for(auto pClient : m_clientsBuff)
            {
                m_clients.push_back(pClient);
            }

            m_clientsBuff.clear();
            _clients_change = true;
        }

        //如果没有客户端，跳出
        if(m_clients.empty())
        {
            //休眠1毫秒
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);

            _oldTime = CELLTime::getNowInMilliSec();

            continue ;
        }


        fd_set fdRead;
        FD_ZERO(&fdRead);
//        这个是主线程再操作的socket，所以不需要读
        FD_SET(m_sock,&fdRead);



        if(_clients_change)
        {

           maxSock = m_clients[0]->m_sockfd;

           //        每次查询以及连接的socket是否有数据需要查收
           for(int i=(int)m_clients.size()-1;i>=0 ;i--){

               FD_SET(m_clients[i]->m_sockfd,&fdRead);

               if(maxSock < m_clients[i]->m_sockfd )
               {
                   maxSock = m_clients[i]->m_sockfd;
               }
           }

            memcpy(&_fdead_back, &fdRead  ,sizeof(fd_set));
           _clients_change = false;

        }
        else {

            memcpy(&fdRead , &_fdead_back ,sizeof(fd_set));
        }


        timeval t ={0,1};

        //all socket +1
        int ret = select(maxSock+1,&fdRead,nullptr,nullptr,&t);   //查询没有数据，等t时间后离开
        if(ret < 0)
        {
            printf("select err!!\n");
            Close();
            return false;
        }
        else if (ret == 0) {
            continue;
        }



        //循环处理加入监听的sock
        for(int i=(int)m_clients.size()-1;i>=0 ;i--){
            if(FD_ISSET(m_clients[i]->m_sockfd,&fdRead))
            {
                //如果sock客户端退出
                if(-1==RecvData(m_clients[i]))
                {
                    auto iter = m_clients.begin() + i; // std::vector<SOCKET>::iterator
                    if(iter != m_clients.end()){

                        //退出事件
                        if(m_pNetEvent){
                             m_pNetEvent->OnNetLeave(m_clients[i]);
                        }

                        _clients_change = true;

//                        delete m_clients[i];
                        m_clients.erase(iter);
                    }
                }
            }

        }


        //check heart
        auto nowTime = CELLTime::getNowInMilliSec();
        auto dt = nowTime - _oldTime;
        _oldTime = nowTime;
        printf("dt==>%d ",(int)dt );
        for(int i=(int)m_clients.size()-1;i>=0 ;i--){

            m_clients[i]->checkSend((int)dt);

            if(m_clients[i]->checkHeart( (int)dt ))
            {
                auto iter = m_clients.begin() + i;

                if(iter != m_clients.end()){

                    //退出事件
                    if(m_pNetEvent){
                        m_pNetEvent->OnNetLeave(m_clients[i]);
                    }

                    //Close();

                    _clients_change = true;
                    m_clients.erase(iter);

                }
            }

        }

    }
   return false;
}


bool CellServer::isRun()
{
    return m_sock != INVALID_SOCKET ;
}


int CellServer::RecvData(ClientSocketPtr pclient)
{

//    //5接受数据
//    int nlen =  (int)recv(pclient->m_sockfd,m_szRecv,RECV_BUFF_SIZE,0);

//    if(nlen <= 0)
//    {
//        printf("client <%d> quit \n",pclient->m_sockfd);
//        return -1;
//    }

//    // recvbuf ==> msgbuf
//    memcpy(pclient->msgBuf() + pclient->getlastMsgPos(),m_szRecv,nlen);


//    pclient->setlastMsgPos(pclient->getlastMsgPos()+nlen);

//    //是否粘包
//    while(pclient->getlastMsgPos() >= sizeof(DataHeader)){

//                printf("RecvData::<%d> \n",(int)pclient->getlastMsgPos());
//         //是否少包
//        //recv data size out of  DataHeader's size ==>  get DataHeader
//        if (pclient->getlastMsgPos()  >=  sizeof(DataHeader))
//        {
//            DataHeader* header =(DataHeader*)pclient->msgBuf();

//            if(pclient->getlastMsgPos() >= header->datalength)
//            {
//                int nSize =  pclient->getlastMsgPos() - header->datalength;

//                OnNetMsg(pclient->m_sockfd ,header);

//                //move pointer to unuse data
//                memcpy(pclient->msgBuf()  , pclient->msgBuf() + header->datalength , nSize);

//                pclient->setlastMsgPos(nSize);
//            }
//        }else {
//            break;
//        }
//    }

    char* szRecv = pclient->msgBuf() + pclient->getlastMsgPos();
    int nLen = (int)recv(pclient->m_sockfd, szRecv, (RECV_BUFF_SIZE)- pclient->getlastMsgPos(), 0);

    //recv jishu
    m_pNetEvent->OnNetRecv(pclient);

    if (nLen <= 0)
    {
        printf("client <%d> quit \n",pclient->m_sockfd);
        return -1;
    }


    pclient->setlastMsgPos(pclient->getlastMsgPos() + nLen);

    count ++;
    while (pclient->getlastMsgPos() >= sizeof(DataHeader))
    {
        DataHeader* header = (DataHeader*)pclient->msgBuf();

        if (pclient->getlastMsgPos() >= header->datalength)
        {

            int nSize = pclient->getlastMsgPos() - header->datalength;

            OnNetMsg(pclient, header);

            memcpy(pclient->msgBuf(), pclient->msgBuf() + header->datalength, nSize);

            pclient->setlastMsgPos(nSize);
        }
        else {

            break;
        }
    }
    return 0;
}

void CellServer::OnNetMsg(ClientSocketPtr pClient, DataHeader* header)
{
    //计数
    m_pNetEvent->OnNetMsg(this,pClient,header);

}

void CellServer::Close()
{
    if(m_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
        for(int  i=(int)m_clients.size()-1;i>=0 ;i--){

            closesocket(m_clients[i]->m_sockfd);
//            delete m_clients[i];
        }

        closesocket(m_sock);

#else
        for(int i=(int)m_clients.size()-1;i>=0 ;i--){

            close(m_clients[i]->m_sockfd);
//            delete m_clients[i];
        }
        close(m_sock);
#endif
        m_sock =INVALID_SOCKET;

        m_clients.clear();

    }

}


void CellServer::addClient(ClientSocketPtr client)
{
    //或者
//        std::lock_guard<std::mutex> lock(m_mutex);
    m_mutex.lock();
    m_clientsBuff.push_back(client);
    m_mutex.unlock();
}

size_t CellServer::getClientSize()
{
    return m_clients.size() + m_clientsBuff.size() ;
}

void CellServer::setEventOj(INetEvent* event)
{
    m_pNetEvent = event;
}


void  CellServer::addSendTask(ClientSocketPtr& pClient, DataHeaderPtr& header)
{
    //    CellSendMag2ClientTaskPtr  _taskPtr = std::make_shared<CellSendMag2ClientTask>(pClient , header);
    //    CellSendMag2ClientTask* _task = new CellSendMag2ClientTask(pClient , header);

    //    匿名函数
    //    可以减少new的操作，提高效率
    m_TaskServer.addTask( [pClient,header] (){
        int ret = pClient->SendData(header);
        printf("aaaaaaaaaaaaaaaaaaaaa===<%d>\n",ret);
    }
    );

}



int CellServer::SendData(SOCKET _csock ,DataHeader* header)
{
    if(isRun() && header){
//        return send(_csock,(const char *)header,sizeof(DataHeader),0);
        return send(_csock,(const char *)header,header->datalength,0);
    }
    return SOCKET_ERROR;
}

void CellServer::SendDataToAll(DataHeader* header)
{
    for(int i=(int)m_clients.size()-1;i>=0 ;i--){
        SendData(m_clients[i]->m_sockfd,header);
    }
}
