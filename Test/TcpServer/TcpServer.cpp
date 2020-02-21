#include "TcpServer.h"



TcpServer::TcpServer()
{
    m_sock =INVALID_SOCKET;
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

        NewLogin newlogin ;
        newlogin.sockId = _csock;
        SendDataToAll((DataHeader* )&newlogin);

        m_clients.push_back(new ClientSocket(_csock));
        printf("new client append : Socket=%d ;IP = %s \n",(int)_csock,inet_ntoa(clientAddr.sin_addr));
    }

    return _csock;
}


void TcpServer::Close()
{
    if(m_sock != INVALID_SOCKET)
    {
#ifdef _WIN32
    for(int  i=(int)m_clients.size()-1;i>=0 ;i--){

        closesocket(m_clients[i]->m_sockfd);
         delete m_clients[i];
    }

    closesocket(m_sock);
    WSACleanup();

#else
    for(int i=(int)m_clients.size()-1;i>=0 ;i--){

        close(m_clients[i]->m_sockfd);
        delete m_clients[i];
    }
    close(m_sock);
#endif
        m_sock =INVALID_SOCKET;


        m_clients.clear();

    }

}

bool TcpServer::OnRun()
{
    if(isRun())
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(m_sock,&fdRead);
        FD_SET(m_sock,&fdWrite);
        FD_SET(m_sock,&fdExp);
        SOCKET maxSock = m_sock;

        //每次查询以及连接的socket是否有数据需要查收
        for(int i=(int)m_clients.size()-1;i>=0 ;i--){
            FD_SET(m_clients[i]->m_sockfd,&fdRead);
            if(maxSock < m_clients[i]->m_sockfd )
            {
               maxSock = m_clients[i]->m_sockfd;
            }
        }

        timeval t ={2,0};
    //        int ret = select(maxSock+1,&fdRead,&fdWrite,&fdExp,NULL);// 阻塞的模式满足应答的模式，如果需要服务端向客户端推送消息需要非阻塞

        //all socket +1
        int ret = select(maxSock+1,&fdRead,&fdWrite,&fdExp,&t);   //查询没有数据，等t时间后离开

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

        //循环处理加入监听的sock
        for(int i=(int)m_clients.size()-1;i>=0 ;i--){
            if(FD_ISSET(m_clients[i]->m_sockfd,&fdRead))
            {
                //如果sock客户端退出
                if(-1==RecvData(m_clients[i]))
                {

                    auto iter = m_clients.begin() + i; // std::vector<SOCKET>::iterator
                    if(iter != m_clients.end()){
                        m_clients.erase(iter);
                    }
                }
            }

        }

    // cout << "Server have time to do something123 !\n" << endl;
        return true;
    }
   return false;

}

bool TcpServer::isRun()
{
    return m_sock != INVALID_SOCKET ;
}

int TcpServer::RecvData(ClientSocket* pclient)
{

    //5接受数据
    int nlen =  (int)recv(pclient->m_sockfd,m_szRecv,RECV_BUFF_SIZE,0);

    // recvbuf ==> msgbuf
    memcpy(pclient->msgBuf() + pclient->getlastMsgPos(),m_szRecv,nlen);


    pclient->setlastMsgPos(pclient->getlastMsgPos()+nlen);

    //是否粘包
    while(pclient->getlastMsgPos() >= sizeof(DataHeader)){

         //是否少包
        //recv data size out of  DataHeader's size ==>  get DataHeader
        if (pclient->getlastMsgPos()  >=  sizeof(DataHeader))
        {
            DataHeader* header =(DataHeader*)pclient->msgBuf();

            if(pclient->getlastMsgPos() >= header->datalength)
            {
                int nSize =  pclient->getlastMsgPos() - header->datalength;

                OnNetMsg(pclient->m_sockfd ,header);

                //move pointer to unuse data
                memcpy(pclient->msgBuf()  , pclient->msgBuf() + header->datalength , nSize);

                pclient->setlastMsgPos(nSize);
            }
        }else {
            break;
        }
    }


    return 0;

}

void TcpServer::OnNetMsg(SOCKET _csock ,DataHeader* header)
{
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

        LoginResult ret;
        ret.result = 1;
        SendData(_csock,(DataHeader*) &ret);

    }
        break;
    case CMD_LOGINOUT:
    {
        LoginOut* loginout = (LoginOut*)header;
        //判断账号密码是否正确
        printf("Server recv cmd=CMD_LOGINOUT ; username= %s ; length=%d\n",loginout->name ,loginout->datalength);

        LoginOutResult outret;
        outret.result = 1;
        SendData(_csock,(DataHeader*) &outret);
    }
        break;

    case CMD_Err:
    {
        DataHeader headererr;
        SendData(_csock,(DataHeader*) &headererr);
    }
        break;
    default:
        printf("server recv cmd=UnDefine \n");
        break;
    }

}

int TcpServer::SendData(SOCKET _csock ,DataHeader* header)
{
    if(isRun() && header){
//        return send(_csock,(const char *)header,sizeof(DataHeader),0);
        return send(_csock,(const char *)header,header->datalength,0);
    }
    return SOCKET_ERROR;
}

void TcpServer::SendDataToAll(DataHeader* header)
{
    for(int i=(int)m_clients.size()-1;i>=0 ;i--){
        SendData(m_clients[i]->m_sockfd,header);
    }
}

