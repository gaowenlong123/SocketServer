#define WIN32_LEAN_AND_MEAN  //windows避免调用之前的宏定义，产生冲突
#include <iostream>
#include <vector>
#include "algorithm"
//windows
#include <windows.h>  //windows系统头文件
#include <winsock2.h>
#include <thread>

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
        cmd = CMD_NEWLOGIN;
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
        cmd = CMD_LOGIN_RESULT;
    }
    int sockId;
};


int procsocket(SOCKET _csock);
void cmdThread(SOCKET _sock);
bool isRun = true;
int main(int argc, char *argv[])
{
    WORD ver = MAKEWORD(2,2);
    WSADATA data;
    WSAStartup(ver, &data);

    //建立一个socket
    SOCKET _sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(INVALID_SOCKET == _sock){
        printf("_sock err!!\n");
    }

    //connect 连接服务器
    sockaddr_in _sin ={};
    _sin.sin_family = AF_INET;
    //我们自己的主机不止一个IP地址
    _sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");   // INADDE_ANY 本机任何的地址都可以访问
    //主机的short 转换到网络中的类型
    _sin.sin_port=htons(9000);

    if(SOCKET_ERROR == connect(_sock,(sockaddr*)&_sin,sizeof(sockaddr_in))){
        printf("connect port err!!\n");
    }else {
        printf("connect port pass!!\n");
    }

    //启动线程
    std::thread t1(cmdThread,_sock);
    t1.detach();

    //输入请求命令 ， 处理请求
    while(isRun)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(_sock,&fdReads);
        timeval t ={5,0};
        int ret = select(_sock,&fdReads ,NULL ,NULL, &t);
        if(ret < 0)
        {
            printf("select err!!\n");
            break;
        }

        if(FD_ISSET(_sock,&fdReads))
        {
            FD_CLR(_sock,&fdReads);

            if(-1 ==procsocket(_sock))
            {
                 printf(" client select quit!!\n");
                 break;
            }
        }


        cout << "Client have time to do something !" << endl;

    }

    //关闭
    closesocket(_sock);
    getchar();
    return 0;
}


void cmdThread(SOCKET _sock)
{
    //输入请求命令 ， 处理请求
    while(true)
    {
        char cmdBuf[128]={};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf,"exit"))
        {
            printf("exit quit !!\n");
            isRun =false;
            break;
        }else if(0 == strcmp(cmdBuf,"login"))
        {
            Login login;
            strcpy(login.name,"gao");
            strcpy(login.password,"123");
            send(_sock,(const char *)&login,sizeof(Login),0);

            //接收消息
            LoginResult ret={};
            recv(_sock,(char *)&ret,sizeof(LoginResult),0);
            printf("LoginResult = %d\n",ret.result);

        }else if(0 == strcmp(cmdBuf,"loginout"))
        {
            LoginOut login;
            strcpy(login.name,"gao");
            send(_sock,(const char *)&login,sizeof(LoginOut),0);

            //接收消息
            LoginOutResult ret={};
            recv(_sock,(char *)&ret,sizeof(LoginOutResult),0);

            printf("LoginOutResult = %d\n",ret.result);

        }else{
             printf("dont support CMD !!\n");
        }
    }

}

int procsocket(SOCKET _csock){

    //缓冲区
    char szRecv[1024]={};

    //5接受客户端的数据
    int nlen =  recv(_csock,szRecv,sizeof(DataHeader),0);
    DataHeader* header =(DataHeader*)szRecv;

    if(nlen<=0){
        printf("client server quit!!\n");
        return -1;
    }

    //是否粘包 或者少包
    if(nlen >= sizeof(DataHeader)){

    }

    // 6处理请求
    switch (header->cmd) {
    case CMD_LOGIN_RESULT:
    {
        //没有拿完值，就在缓冲区中偏移指针
        recv(_csock,szRecv + sizeof(DataHeader),header->datalength- sizeof(DataHeader),0);

        LoginResult* login =(LoginResult*)szRecv;

        //判断账号密码是否正确
        printf("Client recv cmd=CMD_LOGIN_RESULT ; Result=%d" ,login->result);

    }
        break;
    case CMD_LOGINOUT_RESULT:
    {
        recv(_csock,szRecv + sizeof(DataHeader),header->datalength- sizeof(DataHeader),0);
        //判断账号密码是否正确

        LoginOutResult* loginout =(LoginOutResult*)szRecv;
        printf("Server recv cmd=CMD_LOGINOUT_RESULT ; Result=%d" ,loginout->result);


    }
        break;

    case CMD_NEWLOGIN:
    {
        recv(_csock,szRecv + sizeof(DataHeader),header->datalength- sizeof(DataHeader),0);
        //判断账号密码是否正确

        NewLogin* newLogin =(NewLogin*)szRecv;
        printf("Server recv cmd=CMD_NEWLOGIN ; new SOCKID=%d" ,newLogin->sockId);

    }
        break;
    default:
        break;
    }
}
