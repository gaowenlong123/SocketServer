#include <iostream>
#include "MemoryMgr.h"
#include "CellObjPool.h"
#include "memory"
using namespace std;



class ClassA : public ObjectPoolBase<ClassA, 100000>
{
public:
    ClassA(int n)
    {
        num = n;
        printf("ClassA\n");
    }

    ~ClassA()
    {
        printf("~ClassA\n");
    }
public:
    int num = 0;
    char a[1000];
};


int main()
{

    //new才能调用对象里面的new
    {
        ClassA* B1 = new ClassA(0);
        shared_ptr<ClassA> s0 = make_shared<ClassA>(5);

        shared_ptr<ClassA> s1(new ClassA(5));
    }
    printf("----1----\n");
    {
        shared_ptr<ClassA> s1 = make_shared<ClassA>(5);  //
    }

    printf("----2----\n");
    {
        shared_ptr<ClassA> s1(new ClassA(5));    // 可以兼容 ， 在智能的创建的过程中又会给数据加一个头部数据，现在这样是把两者的数据分开创建 ，没有像上面一样把数据合并
    }
    printf("----3----\n");


    ClassA* a1 = new ClassA(5);
    delete a1;
    printf("----4----\n");


    /*
    ----1----
   allocMem: 7ff9348f4010 , id=0 , size=1024
   ClassA
   ~ClassA
   freeMem: 7ff9348f4010 , id=0
    ----2----
   Obj allocMemory: 7ff93530742c, id=1, size=1004
   ClassA
   allocMem: 7ff93c138010 , id=0 , size=24
   ~ClassA
   Obj freeMemory: 7ff93530742c, id=1
   freeMem: 7ff93c138010 , id=0
    ----3----
    */

    return 0;
}


//auto pointer
int main2(int argc, char *argv[])
{
    //普通指针的声明
    int* a = new int;
    delete a;


    //共享指针只在作用域中存在
    //但是运行效率不高，但是稳定性提高 ， 在作用域的结束，可以自己调用析构
    {
        //智能指针的声明  （共享指针）
        std::shared_ptr<int> b = std::make_shared<int>();

        *b = 10;

        cout << "  = "  << *b<< endl;

        *(b.get()) = 20;                   //得到共享指针原本的指针

        cout << "  = "  << *b<< endl;


        cout << "  = "  << b.use_count()<< endl;   //共享指针的引用次数

    }

    cout << "Hello World!" <<endl;
}


int main1(int argc, char *argv[])
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
