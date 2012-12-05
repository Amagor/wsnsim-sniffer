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

INCLUDEPATH += ../libs/qextserialport/src/
INCLUDEPATH += ../libs/globalSettings/realtime/interfaces
INCLUDEPATH += ../libs/globalSettings/realtime/include
INCLUDEPATH += ../libs/globalSettings/realtime/tools/include

win32 {
    LIBS += ../../bin/libglobalSettings.a
    LIBS += ../../bin/libqextserialport-1.2.a
    LIBS += -lsetupapi
}

unix {
    LIBS += -L../../bin -lglobalSettings
    LIBS += -L../../bin -lqextserialport-1.2
}

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
