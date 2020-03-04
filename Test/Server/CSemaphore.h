#ifndef CSEMAPHORE_H
#define CSEMAPHORE_H
#include "chrono"
#include "thread"
#include "iostream"
#include <condition_variable>
#include <mutex>

/*
    CellTaskServer* ce ;

    ce = new CellTaskServer();

    std::cout<<ce->m_isRun<<std::endl;
    ce->StartTask();

    ce->Close();

*/



class CSemaphore
{
public:
    void wait()
    {

        std::unique_lock<std::mutex> lock(m_mutex);

        if(--_wait<0){
            m_block.wait(lock , [this]()-> bool
            {
                             return _wakeup > 0;
                         });
            --_wakeup;
        }



    }

    void wakeup()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if(++_wait <=0){
            ++_wakeup;
            m_block.notify_one();
        }

    }

private:

    std::condition_variable m_block;

    std::mutex m_mutex;


    int _wait = 0;

    int _wakeup = 0;

};

#endif // CSEMAPHORE_H
