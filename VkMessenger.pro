#-------------------------------------------------
#
# Project created by QtCreator 2015-11-07T00:33:06
#
#-------------------------------------------------

TARGET = VkMessenger
TEMPLATE = app
QT       += core gui webenginewidgets  network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11

qtHaveModule(uitools):!embedded: QT += uitools
else: DEFINES += QT_NO_UITOOLS

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
