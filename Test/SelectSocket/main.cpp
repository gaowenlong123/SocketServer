#include <iostream>
#include <vector>
#include "algorithm"
//windows
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN  //windows避免调用之前的宏定义，产生冲突
    #include <windows.h>  //windows系统头文件
    #include <winsock2.h>

#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>


    #define SOCKET  int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR           (-1)
#endif

using namespace std;

enum CMD{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGINOUT,
    CMD_LOGINOUT_RESULT,
    CMD_NEWLOGIN,
    CMD_Err,
};

struct DataHeader
{
    short datalength;
    short cmd;
};

//DataPackage
struct Login :public DataHeader
{
    Login(){
        datalength = sizeof(Login);
        cmd = CMD_LOGIN;
    }

    char name[32];
    char password[32];
};

struct LoginResult:public DataHeader
{
    LoginResult(){
        datalength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
    }
    int result;
};

struct LoginOut :public DataHeader
{
    LoginOut(){
        datalength = sizeof(LoginOut);
        cmd = CMD_LOGINOUT;
    }

    char name[32];
};


struct LoginOutResult:public DataHeader
{
    LoginOutResult(){
        datalength = sizeof(LoginOutResult);
        cmd = CMD_LOGINOUT_RESULT;
    }

    int result;
};

struct NewLogin:public DataHeader
{
    NewLogin(){
        datalength = sizeof(NewLogin);
        cmd = CMD_NEWLOGIN;
    }
    int sockId;
};


vector<SOCKET> g_clients;
int procsocket(SOCKET _csock);
int main(int argc, char *argv[])
{
#ifdef _WIN32
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);
#endif

    //建立一个socket
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //bind 绑定用于接收客户端连接的网络端口
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;

#ifdef _WIN32
    //我们自己的主机不止一个IP地址
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");   // INADDE_ANY 本机任何的地址都可以访问
#else
//    _sin.sin_addr.s_addr= inet_addr("192.168.199.103");
    _sin.sin_addr.s_addr= INADDR_ANY;
#endif

    //主机的short 转换到网络中的类型
    _sin.sin_port=htons(9000);

    if(SOCKET_ERROR == bind(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in))){
        printf("bind port err!!\n");
    }else {
        printf("bind port pass!!\n");
    }

    //listen 监听网络端口
    if(SOCKET_ERROR == listen(_sock,5))
    {      printf("listen port err!!\n");
    }else {
        printf("listen port pass!!\n");
    }


    while (true){
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_sock,&fdRead);
        FD_SET(_sock,&fdWrite);
        FD_SET(_sock,&fdExp);
        SOCKET maxSock = _sock;

        //每次查询以及连接的socket是否有数据需要查收
        for(int i=(int)g_clients.size()-1;i>=0 ;i--){
            FD_SET(g_clients[i],&fdRead);
            if(maxSock < g_clients[i] )
            {
               maxSock = g_clients[i];
            }
        }

        timeval t ={2,0};
//        int ret = select(_sock+1,&fdRead,&fdWrite,&fdExp,NULL);// 阻塞的模式满足应答的模式，如果需要服务端向客户端推送消息需要非阻塞

        //all socket +1
        int ret = select(maxSock+1,&fdRead,&fdWrite,&fdExp,&t);   //查询没有数据，等t时间后离开

        if(ret < 0)
        {
            printf("select err!!\n");
            break;
        }

        if(FD_ISSET(_sock,&fdRead))
        {
            FD_CLR(_sock,&fdRead);

            //4accept 等待接收客户端连接
            sockaddr_in clientAddr ={};
            SOCKET _csock = INVALID_SOCKET;
            int len = sizeof(clientAddr);
#ifdef _WIN32
            _csock = accept(_sock,(sockaddr*)&clientAddr,(int*)&len);
#else
            _csock = accept(_sock,(sockaddr*)&clientAddr,(socklen_t *)&len);
#endif
            if(_csock == INVALID_SOCKET)
            {      printf("accept port err!!\n");
            }

            for(int i=(int)g_clients.size()-1;i>=0 ;i--){
                NewLogin newlogin ;
                send(g_clients[i],(const char*)&newlogin,sizeof(NewLogin),0);
            }

            g_clients.push_back(_csock);
            printf("new client append : Socket=%d ;IP = %s \n",(int)_csock,inet_ntoa(clientAddr.sin_addr));



        }

        //循环处理加入监听的sock
#ifdef _WIN32
        for(size_t i=0;i< fdRead.fd_count ;i++){
            //如果sock客户端退出
            if(-1==procsocket(fdRead.fd_array[i]))
            {
                auto iter = find(g_clients.begin() ,g_clients.end(),fdRead.fd_array[i]);
                if(iter!= g_clients.end()){
                    g_clients.erase(iter);
                }
            }

        }
#else

        for(int i=(int)g_clients.size()-1;i>=0 ;i--){
            if(FD_ISSET(g_clients[i],&fdRead))
            {
                //如果sock客户端退出
                if(-1==procsocket(g_clients[i]))
                {

                    auto iter = g_clients.begin() + i; // std::vector<SOCKET>::iterator
                    if(iter != g_clients.end()){
                        g_clients.erase(iter);
                    }
                }
            }

        }
#endif



//        cout << "Server have time to do something123 !" << endl;
    }

    //关闭
#ifdef _WIN32
    for(int  i=(int)g_clients.size()-1;i>=0 ;i--){

        closesocket(g_clients[i]);
    }

    closesocket(_sock);
    WSACleanup();

#else
    for(int i=(int)g_clients.size()-1;i>=0 ;i--){

        close(g_clients[i]);
    }
    close(_sock);
#endif
    getchar();
    cout << "Hello World123!" << endl;
    return 0;
}


int procsocket(SOCKET _csock){

    //缓冲区
    char szRecv[1024]={};

    //5接受客户端的数据
    int nlen =  (int)recv(_csock,szRecv,sizeof(DataHeader),0);
    DataHeader* header =(DataHeader*)szRecv;

    if(nlen<=0){
        printf("client quit!!\n");
        return -1;
    }

    //是否粘包 或者少包
    if(nlen >= sizeof(DataHeader)){

    }

    // 6处理请求
    switch (header->cmd) {
    case CMD_LOGIN:
    {
        //没有拿完值，就在缓冲区中偏移指针
        recv(_csock,szRecv + sizeof(DataHeader),header->datalength- sizeof(DataHeader),0);

        Login* login =(Login*)szRecv;

        //判断账号密码是否正确
        printf("Server recv cmd=CMD_LOGIN ; username= %s ; length=%d",login->name ,login->datalength);

        LoginResult ret;
        ret.result = 1;
        send(_csock,(char*)&ret,sizeof(LoginResult),0);

    }
        break;
    case CMD_LOGINOUT:
    {
        recv(_csock,szRecv + sizeof(DataHeader),header->datalength- sizeof(DataHeader),0);
        //判断账号密码是否正确

        LoginOut* loginout =(LoginOut*)szRecv;
        printf("Server recv cmd=CMD_LOGINOUT ; username= %s ; length=%d",loginout->name ,loginout->datalength);

        LoginOutResult outret;
        outret.result = 1;
        send(_csock,(char*)&outret,sizeof(LoginOutResult),0);

    }
        break;
    default:
        DataHeader headererr = {0,CMD_Err};
        send(_csock,(char*)&headererr,sizeof(DataHeader),0);
        break;
    }

    return 0;
}


