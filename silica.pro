#-------------------------------------------------
#
# Project created by QtCreator 2013-12-27T22:47:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = silica
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Navigator.cpp \
    Image.cpp \
    PlayList.cpp

HEADERS  += MainWindow.h \
    Navigator.h \
    Image.h \
    PlayList.h

FORMS    += MainWindow.ui
