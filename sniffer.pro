#-------------------------------------------------
#
# Project created by QtCreator 2012-09-07T15:27:32
#
#-------------------------------------------------
include(../wsnsim.pri)
QT       += core gui
QT  +=network

#CONFIG += console

TARGET = sniffer
TEMPLATE = app
#CONFIG += no_icon

include(../libs/qextserialport/src/qextserialport.pri)
LIBS    += $$PWD/../libs/qextserialport/lib/libqextserialport-1.2.a
LIBS    += $$PWD/../libs/qextserialport/lib/libqextserialport-1.2d.a

INCLUDEPATH += ../libs/qextserialport/src/ \
    ../libs/globalSettings/realtime/interfaces/

SOURCES += main.cpp\
        mainwindow.cpp \
    statictools.cpp

HEADERS  += mainwindow.h \
    statictools.h

FORMS    += mainwindow.ui
