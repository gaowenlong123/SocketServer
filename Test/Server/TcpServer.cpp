#include "TcpServer.h"


TcpServer::TcpServer()
{
    m_sock =INVALID_SOCKET;
    _recvCount =0;
    _clentsCount=0;
}

TcpServer::~TcpServer()
{
    Close();
}

int TcpServer::initSocket()
{
    if(m_sock != INVALID_SOCKET)
    {
        printf("Close old _sock !!\n");
        Close();
    }

#ifdef _WIN32
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);
#endif

    //建立一个socket
    m_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(INVALID_SOCKET == m_sock){
        printf("_sock err!!\n");
        return -1;
    }

    return 0;

}

int TcpServer::initBind(const char * ip , unsigned port)
{
    if(m_sock == INVALID_SOCKET)
    {
        initSocket();
    }

    //bind 绑定用于接收客户端连接的网络端口
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;
    //主机的short 转换到网络中的类型
    _sin.sin_port=htons(port);



#ifdef _WIN32
    //我们自己的主机不止一个IP地址
    // INADDE_ANY 本机任何的地址都可以访问
    if(ip)
    {
        _sin.sin_addr.S_un.S_addr= inet_addr(ip);
    }else
    {
        _sin.sin_addr.S_un.S_addr= INADDR_ANY;
    }

#else

    if(ip)
    {
        _sin.sin_addr.s_addr= inet_addr(ip);
    }else
    {
        _sin.sin_addr.s_addr=INADDR_ANY;
    }
#endif


    int ret = bind(m_sock,(sockaddr*)&_sin,sizeof(sockaddr_in));
    if(SOCKET_ERROR ==ret ){
        printf("bind port err!!\n");
    }else {
        printf("bind port pass!!\n");
    }

    return ret;
}

int TcpServer::initListen(int n)
{
    //listen 监听网络端口
    int ret = listen(m_sock,n);
    if(SOCKET_ERROR ==ret )
    {      printf("socket=<%d> listen port err!!\n",(int)m_sock);
    }else {
        printf("listen port pass!!\n");
    }

    return ret;
}

SOCKET TcpServer::socketAccept()
{
        printf("socketAccept \n");
    //4accept 等待接收客户端连接
    sockaddr_in clientAddr ={};
    SOCKET _csock = INVALID_SOCKET;
    int len = sizeof(clientAddr);
#ifdef _WIN32
    _csock = accept(m_sock,(sockaddr*)&clientAddr,(int*)&len);
#else
    _csock = accept(m_sock,(sockaddr*)&clientAddr,(socklen_t *)&len);
#endif
    if(_csock == INVALID_SOCKET)
    {
        printf("accept socket=<%d>  err!!\n",(int)_csock);

    }else {

        //        addClientToCellServer(new ClientSocket(_csock));

        ClientSocketPtr ptr(new  ClientSocket(_csock));
        addClientToCellServer(ptr);

    }

    return _csock;
}

void TcpServer::addClientToCellServer(ClientSocketPtr& pClient)
{
    m_clients.push_back(pClient);

    auto pMinServer = m_cellServers[0];
    for(auto pCellServer : m_cellServers )
    {
        if(pMinServer->getClientSize() > pCellServer->getClientSize() )
        {
            pMinServer = pCellServer;
        }
    }
    pMinServer->addClient(pClient);
    _clentsCount++;
}

void TcpServer::startServerThread()
{
    for(int i=0;i <_CELL_THREAD_COUNT;i++)
    {
//        auto server = new CellServer(m_sock);
//        m_cellServers.push_back(server);
        CellServerPtr server = std::make_shared<CellServer>(m_sock);
        m_cellServers.push_back(server);
        server->setEventOj(this);
        server->Start();
    }
}


void TcpServer::timeForMsg()
{
    //为了测试性能，进行输出响应时间
    auto t1 = m_tTime.getElapsedSecond();
    if(t1 >=1.0)
    {
        printf("time<%lf> , socket<%d> ,clents<%d>, m_recvCount<%d>  _msgCount<%d>\n",t1 ,m_sock , (int)_clentsCount ,(int)(_recvCount/t1) ,(int)_msgCount);
        _recvCount =0;
        _msgCount = 0;
        m_tTime.update();
    }
}

void TcpServer::Close()
{
    if(m_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
    closesocket(m_sock);
    WSACleanup();

#else
    close(m_sock);
#endif
        m_sock =INVALID_SOCKET;
        m_clients.clear();

    }

}

//用来接收客户端连接的select任务
bool TcpServer::OnRun()
{
    //这里我只能只读的
    if(isRun())
    {
        timeForMsg();
        fd_set fdRead;
//        fd_set fdWrite;
//        fd_set fdExp;
        FD_ZERO(&fdRead);
//        FD_ZERO(&fdWrite);
//        FD_ZERO(&fdExp);
        FD_SET(m_sock,&fdRead);
//        FD_SET(m_sock,&fdWrite);
//        FD_SET(m_sock,&fdExp);

        timeval t ={1,0};
    //  int ret = select(maxSock+1,&fdRead,&fdWrite,&fdExp,NULL);// 阻塞的模式满足应答的模式，如果需要服务端向客户端推送消息需要非阻塞

        //all socket +1
        int ret = select(m_sock+1,&fdRead,nullptr,nullptr,&t);   //查询没有数据，等t时间后离开

        if(ret < 0)
        {
            printf("select err!!\n");
            Close();
            return false;
        }

        if(FD_ISSET(m_sock,&fdRead))
        {
            FD_CLR(m_sock,&fdRead);
            socketAccept();
        }
        return true;
    }
   return false;

}

bool TcpServer::isRun()
{
    return m_sock != INVALID_SOCKET ;
}




