TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#windows
#LIBS+=libws2_32

#linux
LIBS += -lpthread


SOURCES += main.cpp \
    TcpClient.cpp

HEADERS += \
    TcpClient.h \
    ../../Public/Define/MessageDefine.h \
    ../../Public/Time/CBaseTime.h \
    ../../Public/Log/CBaseLog.h \
    MyTcpClient.h


