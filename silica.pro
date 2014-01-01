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

INCLUDEPATH += $$PWD/deps/quazip/quazip
DEPENDPATH += $$PWD/deps/quazip/quazip

unix:LIBS += -L$$PWD/deps/quazip/quazip/ -lquazip
unix:PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a
