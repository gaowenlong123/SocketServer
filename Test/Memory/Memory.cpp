#include "MemoryMgr.h"

void* operator new(size_t size)
{
    return MemoryMgr::Instance().alloc_Mgr(size);
}

void operator delete(void* p){
    return MemoryMgr::Instance().free_Mgr(p);
}

void* operator new[](size_t size)
{
     return MemoryMgr::Instance().alloc_Mgr(size);
}

void operator delete[](void* p)
{
    return MemoryMgr::Instance().free_Mgr(p);
}

void* mem_alloc (size_t size)
{
     return MemoryMgr::Instance().alloc_Mgr(size);
}

void mem_free (void* p)
{
    return MemoryMgr::Instance().free_Mgr(p);
}
