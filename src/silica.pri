SOURCES += \
    $$PWD/CommandInterpreter.cpp \
    $$PWD/GlobalConfig.cpp \
    $$PWD/Image.cpp \
    $$PWD/ImagesCache.cpp \
    $$PWD/ImageSource.cpp \
    $$PWD/LocalImageSource.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/Navigator.cpp \
    $$PWD/PlayList.cpp \
    $$PWD/SevenzImageSource.cpp \
    $$PWD/ZipImageSource.cpp \
    $$PWD/db/*.cpp \
    $$PWD/ui/*.cpp


HEADERS += \
    $$PWD/*.h \
    $$PWD/db/*.h \
    $$PWD/ui/*.h \
    $$PWD/util/*.h

INCLUDEPATH += \
    $$PWD \
    $$PWD/db \
    $$PWD/ui \
    $$PWD/util

FORMS += $$PWD/*.ui

RESOURCES += $$PWD/*.qrc

INCLUDEPATH += \
    $$PWD/deps/quazip/quazip \
    $$PWD/deps/Qt7z/Qt7z

DEPENDPATH += \
    $$PWD/deps/quazip/quazip \
    $$PWD/deps/Qt7z/Qt7z

macx {
    LIBS += -L/usr/local/opt/zlib/lib/ -lz

    LIBS += -L$$PWD/deps/quazip/quazip -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a

    LIBS += -L$$PWD/deps/Qt7z/Qt7z -lQt7z
    PRE_TARGETDEPS += $$PWD/deps/Qt7z/Qt7z/libQt7z.a

    config.path = $$OUT_PWD/silica.app/Contents/MacOS/config/Asuna
    config.files = $$PWD/assets/Silica.ini

    INSTALLS += config
}

linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += zlib

    LIBS += -L$$PWD/deps/quazip/quazip/ -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a

    LIBS += -L$$PWD/deps/Qt7z/Qt7z -lQt7z
    PRE_TARGETDEPS += $$PWD/deps/Qt7z/Qt7z/libQt7z.a
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

    # TODO: Add Qt7z

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