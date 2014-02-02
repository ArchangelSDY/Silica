#-------------------------------------------------
#
# Project created by QtCreator 2013-12-27T22:47:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = silica
TEMPLATE = app


SOURCES += main.cpp\
    CommandInterpreter.cpp \
    GlobalConfig.cpp \
    Image.cpp \
    ImagesCache.cpp \
    ImageSource.cpp \
    LocalImageSource.cpp \
    MainWindow.cpp \
    Navigator.cpp \
    PlayList.cpp \
    ZipImageSource.cpp \
    db/Database.cpp \
    db/AsunaDatabase.cpp \
    ui/GalleryItem.cpp \
    ui/GalleryView.cpp \
    ui/MainGraphicsView.cpp

HEADERS  += \
    CommandInterpreter.h \
    GlobalConfig.h \
    Image.h \
    ImagesCache.h \
    ImageSource.h \
    LocalImageSource.h \
    MainWindow.h \
    Navigator.h \
    PlayList.h \
    ZipImageSource.h \
    db/Database.h \
    db/AsunaDatabase.h \
    ui/GalleryItem.h \
    ui/GalleryView.h \
    ui/MainGraphicsView.h

FORMS    += MainWindow.ui

INCLUDEPATH += $$PWD/deps/quazip/quazip
DEPENDPATH += $$PWD/deps/quazip/quazip

DEFINES += QUAZIP_STATIC

RESOURCES += \
    silica.qrc

macx {
    LIBS += -L/usr/local/opt/zlib/lib/ -lz

    LIBS += -L$$PWD/deps/quazip/quazip -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a

    config.path = $$OUT_PWD/silica.app/Contents/MacOS/config/Asuna
    config.files = $$PWD/assets/Silica.ini

    INSTALLS += config
}

linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += zlib

    LIBS += -L$$PWD/deps/quazip/quazip/ -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a
}

win32 {
    DESTDIR = $$OUT_PWD

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

    runtime.path = $$OUT_PWD
    runtime.files = \
        $$[QT_INSTALL_BINS]/Qt5Core.dll \
        $$[QT_INSTALL_BINS]/Qt5Gui.dll \
        $$[QT_INSTALL_BINS]/Qt5Network.dll \
        $$[QT_INSTALL_BINS]/Qt5Widgets.dll \
        $$[QT_INSTALL_BINS]/libEGL.dll \
        $$[QT_INSTALL_BINS]/libGLESv2.dll \
        $$[QT_INSTALL_BINS]/icuin51.dll \
        $$[QT_INSTALL_BINS]/icuuc51.dll \
        $$[QT_INSTALL_BINS]/icudt51.dll

    INSTALLS += runtime

    config.path = $$OUT_PWD/config/Asuna
    config.files = $$PWD/assets/Silica.ini

    INSTALLS += config
}
