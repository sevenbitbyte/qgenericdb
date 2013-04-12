#-------------------------------------------------
#
# Project created by QtCreator 2013-04-05T01:28:16
#
#-------------------------------------------------

QT       += core sql network

QT       -= gui

TARGET = GenericDb
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    database.cpp \
    table.cpp

HEADERS += \
    database.h \
    debug.h
