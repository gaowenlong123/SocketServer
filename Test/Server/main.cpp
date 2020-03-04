#include <iostream>
#include "MyServer.h"
#include "CellTask.h"
using namespace std;

int main(int argc, char *argv[])
{

    MyServer server ;
    server.initSocket();
    server.initBind("127.0.0.1",9000);
    server.initListen(5);
    server.startServerThread();
//    server.socketAccept();



    while(true)
    {
        char cmdBuf[128]={};
        scanf("%s",cmdBuf);
        if(0 == strcmp(cmdBuf,"exit"))
        {
                server.Close();
            break;
        }else{
            printf("dont support CMD !!\n");
        }
    }


    getchar();



    return 0;
}
