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
const int cCount = 12;

//线程数量
const int tCount = 4;

int readyCount = 0;

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


void recvThread(int begin, int end)
{
    while (g_bRun)
    {
        for (int n = begin; n < end; n++)
        {
            client[n]->OnRun();
        }
    }
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
        int ret = client[n]->Connect("127.0.0.1",9000);

        printf("Connect=%d\n",n);

    }

    readyCount ++;
    while(readyCount < tCount)
    {
        //休眠1毫秒
        std::chrono::milliseconds t(10);
        std::this_thread::sleep_for(t);
    }

    //
    std::thread t1(recvThread, begin, end);
    t1.detach();


    Login login[10];

    for (int n = 0; n < 10; n++)
    {
        strcpy(login[n].name, "gao");
        strcpy(login[n].password, "123");
    }

    const int nLen = sizeof(Login);
    while(g_bRun)
    {
        for(int n = begin;n<end;n++)
        {
            if(SOCKET_ERROR != client[n]->SendData((DataHeader*)&login[n] ,nLen ))
            {
                count++;
            }
        }
    }

    for(int n = begin;n<end;n++)
    {
        client[n]->Close();
        delete client[n];
    }
     printf("thread<%d>,exit\n", id);

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
}


