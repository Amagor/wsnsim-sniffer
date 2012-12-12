#-------------------------------------------------
#
# Project created by QtCreator 2012-09-07T15:27:32
#
#-------------------------------------------------
include(../wsnsim.pri)
QT       += core gui \
            network \
            xml



#CONFIG += console

TARGET = sniffer
TEMPLATE = app

CONFIG += qt release
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_DEBUG


include(../libs/qextserialport/src/qextserialport.pri)
LIBS    += $$PWD/../libs/qextserialport/lib/libqextserialport-1.2.a
LIBS    += $$PWD/../libs/qextserialport/lib/libqextserialport-1.2d.a

INCLUDEPATH += ../libs/qextserialport/src/ \
    ../libs/globalSettings/realtime/interfaces/

SOURCES += main.cpp\
        mainwindow.cpp \
    statictools.cpp \
    serialport.cpp \
    transfernetwork.cpp \
    commandhandler.cpp

HEADERS  += mainwindow.h \
    statictools.h \
    transfernetwork.h \
    serialport.h \
    commandhandler.h

FORMS    += mainwindow.ui

RESOURCES +=
