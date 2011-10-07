#-------------------------------------------------
#
# Project created by QtCreator 2011-10-06T01:55:22
#
#-------------------------------------------------

QT       += core

QT       += gui

TARGET = train
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp liblinear-1.8/linear.cpp liblinear-1.8/tron.cpp liblinear-1.8/blas/daxpy.c liblinear-1.8/blas/ddot.c liblinear-1.8/blas/dscal.c liblinear-1.8/blas/dnrm2.c \
    Train.cpp \
    Test.cpp

HEADERS += \
    Train.hpp \
    Test.hpp
