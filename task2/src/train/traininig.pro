QT       += core

QT       += gui

TARGET = training
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app



SOURCES += \
	../src-core/liblinear-1.8/linear.cpp \
	../src-core/liblinear-1.8/tron.cpp \
	../src-core/liblinear-1.8/blas/daxpy.c \
	../src-core/liblinear-1.8/blas/ddot.c \
	../src-core/liblinear-1.8/blas/dscal.c \
	../src-core/liblinear-1.8/blas/dnrm2.c \
	Train.cpp \
	training.cpp \
	../src-core/Instances.cpp

HEADERS += \
	Train.hpp \
	../src-core/Instances.hpp
