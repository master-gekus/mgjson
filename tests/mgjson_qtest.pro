QT       += testlib
QT       -= gui

TARGET = mgjson_qtest

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += MGJSON_AUTOCAST_STRING_VALUES

SOURCES += \
    mgjson_qtest.cpp

include(../mgjson.pri)
