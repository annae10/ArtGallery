QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imageloader.cpp \
    imageloadertask.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    imageloader.h \
    imageloadertask.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH+=C:\OpenSSL-Win64\include
LIBS+=C:\OpenSSL-Win64\bin\libcrypto-3-x64.dll
LIBS+=C:\OpenSSL-Win64\bin\libssl-3-x64.dll
LIBS+=-lws2_32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
