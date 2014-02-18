#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T22:25:04
#
#-------------------------------------------------

QT       += testlib core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += console
CONFIG   -= app_bundle

TARGET = silicatest
TEMPLATE = app

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include(silicatest.pri)
include(../src/silica.pri)
