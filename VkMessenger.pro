#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T00:33:06
#
#-------------------------------------------------

QT       += core gui webkitwidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VkMessenger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    longpoll.cpp \
    session.cpp \
    vkauth.cpp \
    vkrequest.cpp

HEADERS  += mainwindow.h \
    longpoll.h \
    session.h \
    vkauth.h \
    vkrequest.h

FORMS    += mainwindow.ui
