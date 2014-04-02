#-------------------------------------------------
#
# Project created by QtCreator 2013-12-27T22:47:57
#
#-------------------------------------------------

QT       += core gui network sql opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = silica
TEMPLATE = app

include(silica.pri)

SOURCES += main.cpp

DEFINES += QUAZIP_STATIC

macx {
    config.path = $$OUT_PWD/silica.app/Contents/MacOS/config/Asuna
    config.files = $$PWD/assets/Silica.ini

    INSTALLS += config

    migration.path = $$OUT_PWD/silica.app/Contents/MacOS
    migration.files = $$PWD/assets/migration.json

    INSTALLS += migration
}

linux {
    config.path = $$OUT_PWD/config/Asuna
    config.files = $$PWD/assets/Silica.ini

    INSTALLS += config

    migration.path = $$OUT_PWD
    migration.files = $$PWD/assets/migration.json

    INSTALLS += migration
}

win32 {
    # TODO: Add Qt7z
    # TODO: Add QtDBMigration

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

    migration.path = $$OUT_PWD
    migration.files = $$PWD/assets/migration.json

    INSTALLS += migration
}
