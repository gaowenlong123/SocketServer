#ifndef CELLTHREAD_H
#define CELLTHREAD_H
#include "CSemaphore.h"
#include "functional"
#include "iostream"
#include<mutex>
#include "chrono"


class CELLThread
{


private:
    typedef std::function<void(CELLThread*)> EventCall;


public:
    void Start(EventCall onCreate = nullptr,EventCall onRun = nullptr,EventCall onDestory = nullptr)
    {

        std::lock_guard<std::mutex> lock(m_mutex);
        if(!m_isRun)
        {

            m_isRun = true;

            if(onCreate)
                _onCreate = onCreate;
            if(onRun)
                _onRun = onRun;
            if(onDestory)
                _onDestory = onDestory;

            std::thread t(std::mem_fn(&CELLThread::OnWork),this);
            t.detach();

        }

    }

    void Close()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_isRun)
        {
            m_isRun = false;

            m_sem.wait();
        }

    }

    //self kill me
    void Exit()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(m_isRun)
        {
            m_isRun = false;
        }

    }

    bool isRun()
    {
        return m_isRun;
    }

    static void Sleep(int time)
    {
        std::chrono::milliseconds t1(time);
        std::this_thread::sleep_for(t1);
    }

protected:
    void OnWork()
    {
        if(_onCreate)
            _onCreate(this);
        if(_onRun)
             _onRun(this);
        if(_onDestory)
             _onDestory(this);

        m_sem.wakeup();
    }

private:

    EventCall _onCreate;
    EventCall _onRun;
    EventCall _onDestory;


    bool m_isRun = false;

    CSemaphore m_sem;

    std::mutex m_mutex;


};

#endif // CELLTHREAD_H
