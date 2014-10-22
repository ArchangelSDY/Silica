#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T22:25:04
#
#-------------------------------------------------

QT       += testlib core gui network sql opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += console
CONFIG   -= app_bundle

TARGET = silicatest
TEMPLATE = app
QMAKE_MAC_SDK = macosx10.9

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(deps/QtMockWebServer/src/QtMockWebServer.pri)
include(silicatest.pri)
include(../src/silica.pri)
