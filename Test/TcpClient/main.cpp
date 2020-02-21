#include <iostream>
#include "TcpClient.h"
#include <thread>
#include<pthread.h>
using namespace std;

void cmdThread(TcpClient* _sock);
int main(int argc, char *argv[])
{
    TcpClient client;

    client.initSocket();
    //192.168.199.103
    if(-1==client.Connect("127.0.0.1",9000)){
        return -1;
    }


    //启动线程
    std::thread t1(cmdThread,&client);
    t1.detach();


    while(client.isRun())
    {
        client.OnRun();
    }

    client.Close();

    printf("byebye!!\n");
    getchar();
    return 0;
}

void cmdThread(TcpClient* _sock)
{
    //输入请求命令 ， 处理请求
    while(true)
    {
        char cmdBuf[128]={};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf,"exit"))
        {
            printf("exit quit !!\n");
            _sock->Close();
            break;
        }else if(0 == strcmp(cmdBuf,"login"))
        {
            Login login;
            strcpy(login.name,"gao");
            strcpy(login.password,"123");
            _sock->SendData(&login);

        }else if(0 == strcmp(cmdBuf,"loginout"))
        {
            LoginOut login;
            strcpy(login.name,"gao");
            _sock->SendData(&login);

        }else{
             printf("dont support CMD !!\n");
        }
    }

}
