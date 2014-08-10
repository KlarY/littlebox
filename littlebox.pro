#-------------------------------------------------
#
# Project created by QtCreator 2014-08-08T17:38:04
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = littlebox
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    littleboxserver.cpp \
    littleboxthread.cpp \
    littleboxsocket.cpp \
    littleboxdbutil.cpp

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    littleboxserver.h \
    littleboxthread.h \
    littleboxsocket.h \
    littleboxdbutil.h
