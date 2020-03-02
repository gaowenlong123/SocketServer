#ifndef CELLTIMESTAMP_H
#define CELLTIMESTAMP_H
#include <chrono>
#include <iomanip>

class CELLTime
{
public:
    static time_t getNowInMilliSec()
    {
        return getElapsedTimeInMicroSec() * 0.001;
    }



};

class CELLTimestamp
{
public:
    CELLTimestamp()
    {
            update();
    }

    ~CELLTimestamp()
    {

    }

     void update()
     {
        m_begin = std::chrono::high_resolution_clock::now();
     }


     //获取当前秒
     double getElapsedSecond()
     {
         return getElapsedTimeInMicroSec() * 0.000001;
     }

     //获取当前毫秒
     double getElapsedTimeInMilliSec()
     {
         return getElapsedTimeInMicroSec() * 0.001;
     }


     //获取当前微秒
     long long  getElapsedTimeInMicroSec()
     {
         return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();

     }
protected:
//    time_point<high_resolution_clock>

    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;

};

#endif // CELLTIMESTAMP_H
