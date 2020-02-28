#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H


#ifdef _WIN32
    //windows
    #define FD_SETSIZE  1024
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



#define RECV_BUFF_SIZE 10240 //10k
#define SEND_BUFF_SIZE 10240 //10k


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
    DataHeader(){
        datalength = sizeof(DataHeader);
    }

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




#endif // MESSAGEHEADER_H
