#include <iostream>
#include "TcpServer.h"
using namespace std;

int main(int argc, char *argv[])
{

    TcpServer server ;
    server.initSocket();
    server.initBind("127.0.0.1",9001);
    server.initListen(5);
    server.startServerThread();
//    server.socketAccept();


    while(server.isRun()){
        server.OnRun();
    }

    server.Close();
    getchar();

    cout << "Hello World!" << endl;
    return 0;
}
