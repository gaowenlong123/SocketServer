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

    //输入请求命令 ， 处理请求
    while(true)
    {
        char cmdBuf[128]={};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf,"exit"))
        {
            printf("exit quit !!\n");
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

    //关闭
    closesocket(_sock);
    getchar();
    return 0;
}


    //接收服务器的数据 recv
//        char msgBuf[128]={};
//        int nlen = recv(_sock,msgBuf,128,0);
//        if(nlen > 0){
//            DataPackage* dp = (DataPackage*)msgBuf;
//             printf("msgBuf= %d ; %s",dp->age ,dp->name);
//        }else{
//             printf("recv err!!\n");
//        }

