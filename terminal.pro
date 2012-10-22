#-------------------------------------------------
#
# Project created by QtCreator 2012-09-07T15:27:32
#
#-------------------------------------------------

QT       += core gui
QT  +=network

TARGET = terminal
TEMPLATE = app
#CONFIG += no_icon

include($$PWD\\qextserialport-1.2beta1\\src\\qextserialport.pri)
LIBS    += $$PWD\\qextserialport-1.2beta1\\lib\\libqextserialport-1.2.a
LIBS    += $$PWD\\qextserialport-1.2beta1\\lib\\libqextserialport-1.2d.a

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
