TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#windows
LIBS+=libws2_32

#linux
#LIBS += -lpthread


SOURCES += main.cpp \
    TcpServer.cpp \
    ../TcpInterface/CellServer.cpp \
    ../../Public/Memory/Memory.cpp

HEADERS += \
    TcpServer.h \
    ../../Public/Define/MessageDefine.h \
    ../TcpInterface/CellServer.h \
    ../TcpInterface/ClientSocket.h \
    ../../Public/Time/CBaseTime.h \
    ../../Public/Thread/CBaseThread.h \
    ../../Public/Task/CBaseTask.h \
    ../../Public/Stream/CBaseStream.h \
    ../../Public/Semaphore/CSemaphore.h \
    ../../Public/ObjPool/CBaseObjPool.h \
    ../../Public/Memory/Memory.h \
    ../../Public/Memory/MemoryMgr.h \
    ../../Public/Log/CBaseLog.h \
    ../../Public/Event/CBaseEvent.h

SUBDIRS += \
    ../../Public/Memory/Memory.pro
