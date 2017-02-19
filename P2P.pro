TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    communicationserver.cpp \
    peerclient.cpp \
    peerholder.cpp

HEADERS += \
    peerclient.h \
    communicationserver.h \
    peerholder.h \
    peerdata.h
