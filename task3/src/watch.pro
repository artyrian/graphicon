QT       += opengl

TARGET = watch
TEMPLATE = app

SOURCES += main.cpp\
        window.cpp \
    GlWidget.cpp \
    MyWatch.cpp \
    figures.cpp

HEADERS  += window.h \
    GlWidget.h \
    MyWatch.h \
    figures.h \
    resources.h

LIBS += -lglut
