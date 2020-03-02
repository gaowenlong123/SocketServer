#ifndef CELLTASK_H
#define CELLTASK_H

#include<thread>
#include<mutex>
#include<atomic>
#include<list>
#include "functional"
#include <memory.h>
#include "iostream"


//匿名函数
//可以减少new的操作，提高效率
typedef std::function<void()> CellTask;

//执行任务的服务类型
class CellTaskServer
{

public:

    CellTaskServer()
    {

    }

    //虚析构
    ~CellTaskServer()
    {

    }

    void addTask(CellTask task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskBuf.push_back(task);
    }

    void StartTask()
    {
        std::thread  _thread(std::mem_fn(&CellTaskServer::OnRun),this);
        _thread.detach();
    }


private:
    void OnRun()
    {
        while(true)
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


    }



    std::list<CellTask> m_tasks;

    //任务数据缓冲区
    std::list<CellTask> m_taskBuf;

    //改变数据缓冲区需要加锁
    std::mutex m_mutex;

    //线程
    std::thread * m_thread;

};



/*
//任务基类
class CellTask
{
public:
    CellTask()
    {

    }

    //虚析构
    virtual ~CellTask()
    {

    }

    virtual void doTask()
    {

    }

private:

};


typedef std::shared_ptr<CellTask> CellTaskPtr;

//执行任务的服务类型
class CellTaskServer
{

public:

    CellTaskServer()
    {

    }

    //虚析构
    ~CellTaskServer()
    {

    }

    void addTask(CellTaskPtr& task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskBuf.push_back(task);
    }

    void StartTask()
    {
        std::thread  _thread(std::mem_fn(&CellTaskServer::OnRun),this);
        _thread.detach();
    }


private:
    void OnRun()
    {
        while(true)
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
                pTask.get()->doTask();
            }

            for(auto pTask :m_tasks)
            {
//                delete pTask;
            }

            m_tasks.clear();
        }


    }



    std::list<CellTaskPtr> m_tasks;

    //任务数据缓冲区
    std::list<CellTaskPtr> m_taskBuf;

    //改变数据缓冲区需要加锁
    std::mutex m_mutex;

    //线程
    std::thread * m_thread;

};

*/
#endif // CELLTASK_H
