#ifndef MEMORYMGR_H
#define MEMORYMGR_H

#include <stdlib.h>
#include <assert.h>


/*
            | header              |     64kb用户实际操作内存块       |
            | (MemoryBlock)       |     64kb用户实际操作内存块       |
            |                     |     64kb用户实际操作内存块       |

*/

#define MAX_MEMORY_SIZE 64


//内存块 最小单元
class MemoryAlloc;
class MemoryBlock
{
public:
    MemoryBlock()
    {

    }

    ~MemoryBlock()
    {

    }

    //内存块编号
    int nID;

    //引用次数
    int nRef;

    //所属内存池
    MemoryAlloc* pAlloc;

    //下一块的位置
    MemoryBlock* pNext;

    //是否在内存池中
    bool bPool;


private:

    //补齐32字节  ，指针根据操作系统的不同可能是8字节也可能是4字节 ， 存储的方式也可能不同4字节对齐，还有8字节对齐
    int c1;
    int c2;
    int c3;

//    char c1;
//    char c2;
//    char c3;
};


//内存池
class MemoryAlloc
{

public:
    MemoryAlloc()
    {
        _pBuf = nullptr;

        _pHeader = nullptr;

        _nSize=0;

        _nBlockSzie=0;

    }

    ~MemoryAlloc()
    {
        if(_pBuf)
            free(_pBuf);
    }

    //申请内存
    void* allocMem(size_t nSize)
    {
        if(!_pBuf)
            initMemory() ;

        //单独向系统申请内存
        MemoryBlock* _pReturn = nullptr;


        if(_pHeader == nullptr)
        {
            _pReturn  = (MemoryBlock*)malloc(nSize +  sizeof(MemoryBlock));
            _pReturn->bPool = false;
            _pReturn->nID = -1;
            _pReturn->nRef = 1;
            _pReturn->pAlloc = this;
            _pReturn->pNext = nullptr;
        }else{
            _pReturn = _pHeader;
            _pHeader = _pHeader->pNext;
            assert(0==_pReturn->nRef);
            _pReturn->nRef  =1;

        }

        return ((char*)_pReturn + sizeof(MemoryBlock));
    }

    //释放  只是释放头部指针
    void freeMem(void* pmem)
    {
        char* pData = (char*)pmem;
        MemoryBlock* pBlock = (MemoryBlock*) (pData - (sizeof(MemoryBlock)));
        assert(1==pBlock->nRef);

        //兼容共享内存
        if(--pBlock->nRef != 0)
        {
            return;
        }

        if(pBlock->bPool)
        {
             pBlock->pNext = _pHeader;
             _pHeader = pBlock;

        }else{
              free(pBlock);
        }
    }

    //初始化
    void initMemory()
    {
        //断言 不满足条件，抛出错误
        assert(nullptr==_pBuf);
        if(_pBuf)
            return ;

        //向系统申请内存
        size_t nSize =(_nSize + sizeof(MemoryBlock)) *_nBlockSzie;
        _pBuf = (char*)malloc(nSize);


        //初始化内存池
        _pHeader = (MemoryBlock*)_pBuf;
        _pHeader->bPool = true;
        _pHeader->nID = 0;
        _pHeader->nRef = 0;
        _pHeader->pAlloc = this;
        _pHeader->pNext = nullptr;

        MemoryBlock* _upTemp = _pHeader;
        for(size_t n = 1;n<_nBlockSzie;n++)
        {
             MemoryBlock* _pTemp = (MemoryBlock*)(_pBuf + n*_nSize);
             _pTemp->bPool = true;
             _pTemp->nID = n;
             _pTemp->nRef = 0;
             _pTemp->pAlloc = this;

             _upTemp->pNext = _pTemp;

             _upTemp = _pTemp;
        }

    }

protected:

    //内存池地址
    char* _pBuf;

    //当前可用内存单元块的头部指针地址 （头部是指用户数据操作块的头部 ）
    MemoryBlock* _pHeader;

    //内存单元大小
    size_t _nSize;

    //内存单元的数量
    size_t _nBlockSzie;


};


template<size_t nSzie ,size_t nBlockSzie>
class MemoryAlloctor : public MemoryAlloc
{

public:
    MemoryAlloctor()
    {
        const size_t  n = sizeof(void*);
        _nSize = (_nSize / n)*n + (_nSize % n ? n:0);   //(_nSize % n ? n:0)  先取余 ，返回大于0为真，就返回n 否则返回0
        _nSize = nSzie;
        _nBlockSzie = nBlockSzie;
    }

};


//内存管理工具
class MemoryMgr
{

private:
    MemoryMgr()
    {
        init(0,64,&_mem64);
    }

    ~MemoryMgr()
    {

    }

public:

    //后续再优化单例模式
    static MemoryMgr& Instance()
    {
        static MemoryMgr mgr;
        return mgr;
    }

    //申请内存的方法  (当你new的时候，通过重载运算符得到size大小，然后重定向到这里来
    void* alloc_Mgr(size_t nSize)
    {
        if(nSize <= MAX_MEMORY_SIZE)
        {
            return     _szAlloc[nSize]->allocMem(nSize);
        }
        else
        {
            MemoryBlock* _pReturn  = (MemoryBlock*)malloc(nSize +  sizeof(MemoryBlock));
            _pReturn->bPool = false;
            _pReturn->nID = -1;
            _pReturn->nRef = 1;
            _pReturn->pAlloc = nullptr;
            _pReturn->pNext = nullptr;

            return  ((char*)_pReturn + sizeof(MemoryBlock));
        }

    }

    //释放内存的方法
    void free_Mgr(void* pMem)
    {
        MemoryBlock* pBlock = (MemoryBlock*) ((char*)pMem - (sizeof(MemoryBlock)));
        assert(1==pBlock->nRef);

        //兼容共享内存

        if(pBlock->bPool)
        {
             pBlock->pAlloc->freeMem(pMem);
        }else{
            if(--pBlock->nRef == 0)
              free(pBlock);
        }
    }

    //增加内存引用
    void addRef(void* pMem)
    {
        MemoryBlock* pBlock = (MemoryBlock*) ((char*)pMem - (sizeof(MemoryBlock)));

        pBlock->nRef ++;
    }

private:

    //初始化内存池映射数组
    void init(int nBegin , int nEnd ,MemoryAlloc* pMemA)
    {
        for(int n =nBegin ; n <= nEnd; n++)
        {
            _szAlloc[n] = pMemA;
        }
    }


    MemoryAlloctor<64,10> _mem64;
    MemoryAlloctor<128,10> _mem128;
    MemoryAlloctor<1024,10> _mem1024;

    MemoryAlloc*  _szAlloc[MAX_MEMORY_SIZE+1];

};

#endif // MEMORYMGR_H
