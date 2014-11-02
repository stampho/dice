#-------------------------------------------------
#
# Project created by QtCreator 2014-09-06T21:44:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dice
TEMPLATE = app


SOURCES += main.cpp\
        dicewindow.cpp \
    API/cube.cpp \
    API/dimage.cpp \
    API/face.cpp \
    imageexplorer.cpp \
    imageloader.cpp \
    imagestack.cpp

HEADERS  += dicewindow.h \
    API/core.h \
    API/cube.h \
    API/dimage.h \
    API/face.h \
    imageexplorer.h \
    imageloader.h \
    imagestack.h

FORMS    += dicewindow.ui

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}
