QT       += core gui
QT += core network
QT += charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpuclient.cpp \
    cpuserver.cpp \
    main.cpp \
    memclient.cpp \
    memserver.cpp \
    windowclient.cpp \
    windowserver.cpp

HEADERS += \
    cpuclient.h \
    cpuserver.h \
    memclient.h \
    memserver.h \
    windowclient.h \
    windowserver.h


FORMS += \
    windowclient.ui \
    windowserver.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
