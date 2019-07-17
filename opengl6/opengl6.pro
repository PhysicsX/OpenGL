TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


SOURCES += /home/ulas/soil/src/SOIL.c
SOURCES += /home/ulas/soil/src/stb_image_aug.c
SOURCES += /home/ulas/soil/src/image_helper.c
SOURCES += /home/ulas/soil/src/image_DXT.c


LIBS += -lX11
LIBS += -lpthread
LIBS += -lGL
