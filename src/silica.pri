SOURCES += \
    $$PWD/CommandInterpreter.cpp \
    $$PWD/GlobalConfig.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/Navigator.cpp \
    $$PWD/PlayList.cpp \
    $$PWD/db/*.cpp \
    $$PWD/image/*.cpp \
    $$PWD/image/sources/*.cpp \
    $$PWD/ui/*.cpp \
    $$PWD/ui/renderers/*.cpp

HEADERS += \
    $$PWD/*.h \
    $$PWD/db/*.h \
    $$PWD/image/*.h \
    $$PWD/image/sources/*.h \
    $$PWD/ui/*.h \
    $$PWD/ui/renderers/*.h \
    $$PWD/util/*.h

INCLUDEPATH += \
    $$PWD \
    $$PWD/db \
    $$PWD/image \
    $$PWD/image/sources \
    $$PWD/ui \
    $$PWD/ui/renderers \
    $$PWD/util

FORMS += $$PWD/*.ui

RESOURCES += $$PWD/*.qrc

INCLUDEPATH += \
    $$PWD/deps/quazip/quazip \
    $$PWD/deps/Qt7z/Qt7z \
    $$PWD/deps/QtDBMigration/src

DEPENDPATH += \
    $$PWD/deps/quazip/quazip \
    $$PWD/deps/Qt7z/Qt7z \
    $$PWD/deps/QtDBMigration/src

macx {
    LIBS += -L/usr/local/opt/zlib/lib/ -lz

    LIBS += -L$$PWD/deps/quazip/quazip -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a

    LIBS += -L$$PWD/deps/Qt7z/Qt7z -lQt7z
    PRE_TARGETDEPS += $$PWD/deps/Qt7z/Qt7z/libQt7z.a

    LIBS += -L$$PWD/deps/QtDBMigration/src -lQtDBMigration
    PRE_TARGETDEPS += $$PWD/deps/QtDBMigration/src/libQtDBMigration.a
}

linux {
    CONFIG += link_pkgconfig
    PKGCONFIG += zlib

    LIBS += -L$$PWD/deps/quazip/quazip/ -lquazip
    PRE_TARGETDEPS += $$PWD/deps/quazip/quazip/libquazip.a

    LIBS += -L$$PWD/deps/Qt7z/Qt7z -lQt7z
    PRE_TARGETDEPS += $$PWD/deps/Qt7z/Qt7z/libQt7z.a

    LIBS += -L$$PWD/deps/QtDBMigration/src -lQtDBMigration
    PRE_TARGETDEPS += $$PWD/deps/QtDBMigration/src/libQtDBMigration.a
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
}
