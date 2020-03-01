TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#windows
#LIBS+=libws2_32

#linux
LIBS += -lpthread


SOURCES += main.cpp \
    TcpServer.cpp \
    CellServer.cpp \
    ../Memory/Memory.cpp

HEADERS += \
    TcpServer.h \
    MessageHeader.h \
    ClientSocket.h \
    CELLTimestamp.h \
    INetEvent.h \
    inetevent.h \
    CellTimestamp.h \
    CellServer.h \
    MyServer.h \
    CellTask.h \
    ../Memory/Memory.h \
    ../Memory/MemoryMgr.h \
    ../Memory/CellObjPool.h
