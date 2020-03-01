#include <iostream>
#include "MemoryMgr.h"
using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World!" << sizeof(MemoryBlock)<< endl;

    //1
//    char* data1 = new char[128];
//    delete[] data1;


//    char* data2 = new char;
//    delete data2;


//    char* data3 = new char[64];
//    delete[] data3;

    //2
    char* data[1100];
    for(size_t i =0;i < 1100 ;i++)
    {
        data[i] = new char[i+1];
    }

    for(size_t i =0;i < 1100 ;i++)
    {
        delete[] data[i];
    }



    return 0;
}
