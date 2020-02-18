#define WIN32_LEAN_AND_MEAN  //windows避免调用之前的宏定义，产生冲突
#include <iostream>

//windows
#include <windows.h>  //windows系统头文件
#include <winsock2.h>

using namespace std;

enum CMD{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGINOUT,
    CMD_LOGINOUT_RESULT,
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


int main(int argc, char *argv[])
{
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);

    //建立一个socket
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //bind 绑定用于接收客户端连接的网络端口
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;
    //我们自己的主机不止一个IP地址
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");   // INADDE_ANY 本机任何的地址都可以访问
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

    //4accept 等待接收客户端连接
    sockaddr_in clientAddr ={};
    SOCKET _csock = INVALID_SOCKET;
    int len = sizeof(clientAddr);

    while(true){
        _csock = accept(_sock,(sockaddr*)&clientAddr,(int*)&len);
        if(_csock == INVALID_SOCKET)
        {      printf("accept port err!!\n");
        }

        printf("new client append:IP = %s \n",inet_ntoa(clientAddr.sin_addr));
        //send 向客户端发送一条数据请求

        while (true){

            //缓冲区
            char szRecv[1024]={};

            //5接受客户端的数据
            int nlen =  recv(_csock,szRecv,sizeof(DataHeader),0);
            DataHeader* header =(DataHeader*)szRecv;

            if(nlen<=0){
                printf("client quit!!\n");
                break;
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

        }

    }

    //关闭
    closesocket(_sock);


    WSACleanup();
    getchar();
    cout << "Hello World123!" << endl;
    return 0;
}























/*
//单个连接
int main1(int argc, char *argv[])
{
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);

    //建立一个socket
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //bind 绑定用于接收客户端连接的网络端口
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;
    //我们自己的主机不止一个IP地址
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");   // INADDE_ANY 本机任何的地址都可以访问
    //主机的short 转换到网络中的类型
    _sin.sin_port=htons(6789);

    if(SOCKET_ERROR == bind(_sock,(sockaddr*)&_sin,sizeof(_sin))){
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

    //accept 等待接收客户端连接
    sockaddr_in clientAddr ={};
    SOCKET _csock = INVALID_SOCKET;
    int len = sizeof(clientAddr);
    _csock = accept(_sock,(sockaddr*)&clientAddr,(int*)&len);
    if(_csock == INVALID_SOCKET)
    {      printf("accept port err!!\n");
    }

    //send 向客户端发送一条数据请求
    char msgBuf[128]="Hello,I am Server\n";

    send(_sock,msgBuf,128,0);  // +1  加上结尾符

    //关闭
    closesocket(_sock);


    WSACleanup();

    cout << "Hello World!" << endl;
    return 0;
}
*/
