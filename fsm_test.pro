TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    util.h \
    cvector.h \
    movingobject.h \
    world.h \
    bomb.h \
    aila.h \
    fsm.h \
    fsm_bombing.h
