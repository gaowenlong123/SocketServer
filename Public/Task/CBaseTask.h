#ifndef CBASETASK_H
#define CBASETASK_H

#include<thread>
#include<mutex>
#include<atomic>
#include<list>
#include "functional"
#include <memory.h>
#include "iostream"

#include "../../Public/Thread/CBaseThread.h"

//匿名函数
//可以减少new的操作，提高效率
typedef std::function<void()> CellTask;

//执行任务的服务类型
class CBaseTaskServer
{

public:

    CBaseTaskServer()
    {

    }

    //虚析构
    ~CBaseTaskServer()
    {
//       Close();
    }

    void addTask(CellTask task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskBuf.push_back(task);
    }

    void StartTask()
    {

//        std::thread  _thread(std::mem_fn(&CellTaskServer::OnRun),this);
//        _thread.detach();

        m_thread.Start(nullptr,[this](CBaseThread* pThread){OnRun(pThread);},nullptr);
    }

    void Close()
    {

            printf("Close CellTask start\n");

//            m_isRun = false;
//            m_sem.wait();

            m_thread.Close();

            printf("Close CellTask end\n");

    }


private:
    void OnRun(CBaseThread* pThread)
    {
        while(pThread->isRun())
        {
            if(!m_taskBuf.empty())
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                for(auto pTask :m_taskBuf)
                {
                    m_tasks.push_back(pTask);
                }
                m_taskBuf.clear();
            }

            if(m_tasks.empty())
            {
                std::chrono::milliseconds t(2);
                std::this_thread::sleep_for(t);
                continue;
            }

            //处理任务
            for(auto pTask :m_tasks)
            {
                pTask();
            }

            m_tasks.clear();
        }

        printf("CellTask OnRun exit \n");
//        m_sem.wakeup();
    }



    std::list<CellTask> m_tasks;

    //任务数据缓冲区
    std::list<CellTask> m_taskBuf;

    //改变数据缓冲区需要加锁
    std::mutex m_mutex;

    //线程
    CBaseThread m_thread;

};



#endif // CBASETASK_H
