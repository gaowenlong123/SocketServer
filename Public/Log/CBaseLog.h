#ifndef CBASELOG_H
#define CBASELOG_H

#include "../../Public/Task/CBaseTask.h"
#include "stdlib.h"
#include "ctime"
#include <chrono>
#include "iostream"



class CBaseLog
{
public:
    CBaseLog()
    {

        m_taskServer.StartTask();
    }

    ~CBaseLog()
    {
        m_taskServer.Close();

        if(_logFile)
        {
            fclose(_logFile);
            _logFile = nullptr;
        }

    }


    static CBaseLog& Instance()
    {
        static CBaseLog sLog;
        return sLog;
    }


    static void Info(const char* pStr)
    {


        CBaseLog* log = &Instance();


        log->m_taskServer.addTask([=](){
            if(log->_logFile)
            {
                auto t = std::chrono::system_clock::now();
                auto tNow = std::chrono::system_clock::to_time_t(t);
    //            fprintf(log->_logFile , "%s", ctime(&tNow) );
                std::tm* now = std::gmtime(&tNow);

                fprintf(log->_logFile , "[%d-%d-%d %d:%d:%d]", now->tm_year+1900,now->tm_mon+1,now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

                fprintf(log->_logFile , "%s", pStr );
                fflush(log->_logFile);

            }

            printf(pStr);
        });

    }

    template<typename ...Args>
    static void Info(const char* pformat , Args ... args)
    {

        CBaseLog* log = &Instance();
        log->m_taskServer.addTask([=](){

            if(log->_logFile)
            {

                auto t = std::chrono::system_clock::now();
                auto tNow = std::chrono::system_clock::to_time_t(t);
                std::tm* now = std::gmtime(&tNow);

                fprintf(log->_logFile , "%s", "info " );

                fprintf(log->_logFile , "[%d-%d-%d %d:%d:%d]", now->tm_year+1900,now->tm_mon+1,now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

                fprintf(log->_logFile , pformat, args... );
                fflush(log->_logFile);

            }
            printf(pformat , args...);

        });


    }


    void setLogPath(const char* logpath , const char* mode)
    {
        if(_logFile)
        {
            fclose(_logFile);
            _logFile = nullptr;
        }

        _logFile = fopen(logpath , mode);

        if(_logFile)
        {
            Info("setlogpath ok");
        }
    }



    void Debug(const char* pStr)
    {

    }



    void Warring(const char* pStr)
    {

    }



    void Error(const char* pStr)
    {

    }



private:
    FILE* _logFile = nullptr;
    CBaseTaskServer m_taskServer;
};

#endif // CBASELOG_H
