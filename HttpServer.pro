QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle
QT = core network
QT +=sql
QT += concurrent
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/controller/getdepartcontroller.cpp \
        src/controller/historicaldatacontroller.cpp \
        src/controller/personnumbercontroller.cpp \
        src/controller/realtimedepartconcretecontroller.cpp \
        src/controller/realtimedepartcontroller.cpp \
        src/controller/setcontroller.cpp \
        src/controller/summarizedatacontroller.cpp \
        src/main.cpp \
        src/requestmapper.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/controller/getdepartcontroller.h \
    src/controller/historicaldatacontroller.h \
    src/controller/personnumbercontroller.h \
    src/controller/realtimedepartconcretecontroller.h \
    src/controller/realtimedepartcontroller.h \
    src/controller/setcontroller.h \
    src/controller/summarizedatacontroller.h \
    src/requestmapper.h
include(./logging/logging.pri)
include(./httpserver/httpserver.pri)
include(./templateengine/templateengine.pri)
include(./datadeal/dataconsole.pri)

OTHER_FILES += etc/* etc/docroot/* etc/templates/* etc/ssl/* logs/* ../readme.txt



win32: LIBS += -L$$PWD/lib/VS2015/ -lHttpUtillib

INCLUDEPATH += $$PWD/include/HttpUtillib
DEPENDPATH += $$PWD/include/HttpUtillib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/VS2015/HttpUtillib.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/lib/VS2015/libHttpUtillib.a
