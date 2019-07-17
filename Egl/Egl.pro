TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


LIBS += -lX11
LIBS += -lpthread
LIBS += -lGL
LIBS += -L/usr/lib/x86_64-linux-gnu/libGL.so
