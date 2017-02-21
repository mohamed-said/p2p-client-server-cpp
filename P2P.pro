TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    communicationserver.cpp \
    peerclient.cpp \
    peerholder.cpp \
    server/communicationserver.cpp \
    server/main.cpp \
    server/peerholder.cpp

HEADERS += \
    peerclient.h \
    communicationserver.h \
    peerholder.h \
    peerdata.h \
    server/communicationserver.h \
    server/peerdata.h \
    server/peerholder.h

DISTFILES += \
    README.md
