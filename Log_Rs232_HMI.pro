QT       += core gui serialport sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    filter.cpp \
    localdb.cpp \
    logrecord.cpp \
    main.cpp \
    mainwindow.cpp \
    nozzle.cpp \
    receiverthread.cpp \
    scada.cpp \
    serialport.cpp \
    serialport485.cpp

HEADERS += \
    filter.h \
    localdb.h \
    logrecord.h \
    mainwindow.h \
    nozzle.h \
    nozzlehelper.h \
    receiverthread.h \
    scada.h \
    serialport.h \
    serialport485.h

FORMS += \
    filter.ui \
    mainwindow.ui \
    scada.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
