#include <iostream>
#include "TcpServer.h"
using namespace std;

int main(int argc, char *argv[])
{

    TcpServer server ;
    server.initSocket();
    server.initBind(nullptr,9000);
    server.initListen(5);
    server.socketAccept();


    while(server.isRun()){
        server.OnRun();
    }

    server.Close();
    getchar();

    cout << "Hello World!" << endl;
    return 0;
}
