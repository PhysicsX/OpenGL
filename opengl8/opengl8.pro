TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    helper.cpp

LIBS += -lX11 -lGL -lGLU -lpthread
