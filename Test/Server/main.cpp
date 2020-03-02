#include <iostream>
#include "MyServer.h"
using namespace std;

int main(int argc, char *argv[])
{

    MyServer server ;
    server.initSocket();
    server.initBind("127.0.0.1",9000);
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
