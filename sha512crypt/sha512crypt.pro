#-------------------------------------------------
#
# Project created by QtCreator 2014-07-04T20:55:50
#
#-------------------------------------------------

QT       -= core gui

TARGET = sha512crypt
TEMPLATE = lib
CONFIG += staticlib -std=gnuc99

SOURCES += sha512crypt.cpp

HEADERS += sha512crypt.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
QMAKE_CXXFLAGS += -D__STDC_CONSTANT_MACROS
QMAKE_CXXFLAGS += -fpermissive
