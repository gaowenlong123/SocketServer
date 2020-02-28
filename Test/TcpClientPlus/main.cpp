#include <iostream>
#include "TcpClient.h"
#include <thread>
#include<pthread.h>
#include <celltimestamp.h>
#include <atomic>

using namespace std;

void cmdThread();
void sendThread(int id);

bool g_bRun = true;

//客户端数量
const int cCount = 200;

//线程数量
const int tCount = 4;

//客户端数组
TcpClient* client[cCount];

std::atomic_int count;

int main(int argc, char *argv[])
{
    //启动UI线程
    std::thread t1(cmdThread);
    t1.detach();

    //启动发送线程
    for(int n=0 ;n < tCount;n++)
    {
        std::thread t(sendThread,n+1);
        t.detach();
    }

    CELLTimestamp tTime;
    while(g_bRun)
    {

        auto t = tTime.getElapsedSecond();
        if(t >= 1.0)
        {
            printf("thread<%d> , clients<%d> ,time<%lf>,send<%d> \n" , tCount ,cCount , t ,(int)count);
            tTime.update();
            count=0;
        }

        std::chrono::milliseconds t1(1000);
        std::this_thread::sleep_for(t1);

    }



    printf("byebye!!\n");
    return 0;
}


void sendThread(int id)
{
    int c = cCount / tCount;
    int begin = (id-1)*c;
    int end = id*c;

    for(int n = begin;n<end;n++)
    {
        client[n] = new TcpClient();

    }

    for(int n = begin;n<end;n++)
    {
        int ret = client[n]->Connect("127.0.0.1",9001);

        printf("Connect=%d\n",n);

    }


    //休眠1毫秒
    std::chrono::milliseconds t(1000);
    std::this_thread::sleep_for(t);


    Login login;
    strcpy(login.name, "gao");
    strcpy(login.password, "123");

    while(g_bRun)
    {
        for(int n = begin;n<end;n++)
        {
            if(SOCKET_ERROR != client[n]->SendData(&login))
            {
                count++;
            }
        }
    }

    for(int n = begin;n<end;n++)
    {
        client[n]->Close();
    }

}

void cmdThread()
{

    while(true)
    {
        char cmdBuf[128]={};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf,"exit"))
        {
            g_bRun = false;
            break;
        }else{
            printf("dont support CMD !!\n");
        }
    }
    //输入请求命令 ， 处理请求
    //    while(true)
    //    {
    //        char cmdBuf[128]={};
    //        scanf("%s",cmdBuf);
    //        if(0 == strcmp(cmdBuf,"exit"))
    //        {
    //            printf("exit quit !!\n");
    //            _sock->Close();
    //            break;
    //        }else if(0 == strcmp(cmdBuf,"login"))
    //        {
    //            Login login;
    //            strcpy(login.name,"gao");
    //            strcpy(login.password,"123");
    //            _sock->SendData(&login);

    //        }else if(0 == strcmp(cmdBuf,"loginout"))
    //        {
    //            LoginOut login;
    //            strcpy(login.name,"gao");
    //            _sock->SendData(&login);

    //        }else{
    //             printf("dont support CMD !!\n");
    //        }
    //    }



}
