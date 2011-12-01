#-------------------------------------------------
#
# Project created by QtCreator 2011-11-28T23:00:28
#
#-------------------------------------------------

QT       += core gui

TARGET = RayTrace
TEMPLATE = app

QMAKE_LFLAGS += -fopenmp

SOURCES += main.cpp\
        RayTrace.cpp \
    scene1.cpp \
    Geometry.cpp \
    Object.cpp

HEADERS  += RayTrace.h \
    scene1.h \
    Geometry.h \
    Object.h
