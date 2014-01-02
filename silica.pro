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

DEFINES += QUAZIP_STATIC

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += zlib

    LIBS += -L$$PWD/deps/quazip/quazip/ -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a
}

win32 {
    INCLUDEPATH += $$PWD/deps/zlib/include
    DEPENDPATH += $$PWD/deps/zlib/include

    LIBS += -L$$PWD/deps/zlib/lib/ -lzlib
    PRE_TARGETDEPS += $$PWD/deps/zlib/lib/zlib.lib

    CONFIG(release, debug|release) {
        LIBS += -L$$PWD/deps/build-quazip-win32-release/quazip/release/ -lquazip
        PRE_TARGETDEPS += $$PWD/deps/build-quazip-win32-release/quazip/release/quazip.lib
    }

    CONFIG(debug, debug|release) {
        LIBS += -L$$PWD/deps/build-quazip-win32-debug/quazip/debug/ -lquazip
        PRE_TARGETDEPS += $$PWD/deps/build-quazip-win32-debug/quazip/debug/quazip.lib
    }
}
