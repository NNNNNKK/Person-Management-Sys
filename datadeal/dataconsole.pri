QT += core
QT -= gui
QT += remoteobjects
QT +=sql
QT += axcontainer
QT += concurrent

# Enable very detailed debug messages when compiling the debug version
# Enable very detailed debug messages when compiling the debug version
HEADERS += $$PWD/connectionpool.h\
           $$PWD/databasethread.h\
           $$PWD/departset.h \
    $$PWD/historydatadeal.h \
    $$PWD/realtimedataprocess.h \
           $$PWD/systemconfig.h\
           $$PWD/contraller.h\ \
    $$PWD/threadcontrol.h

SOURCES+=  $$PWD/connectionpool.cpp\
           $$PWD/databasethread.cpp\
           $$PWD/departset.cpp \
    $$PWD/historydatadeal.cpp \
    $$PWD/realtimedataprocess.cpp \
           $$PWD/systemconfig.cpp\
           $$PWD/contraller.cpp\ \
    $$PWD/threadcontrol.cpp

