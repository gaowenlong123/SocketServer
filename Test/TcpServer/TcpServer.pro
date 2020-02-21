TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


#windows
LIBS+=libws2_32

#linux
#LIBS += -lpthread


SOURCES += main.cpp \
    TcpServer.cpp

HEADERS += \
    TcpServer.h \
    MessageHeader.h \
    ClientSocket.h \
    CELLTimestamp.h
