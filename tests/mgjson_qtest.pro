QT       += testlib
QT       -= gui

TARGET = mgjson_qtest

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += MGJSON_AUTOCAST_STRING_VALUES

*-clang || *-g++ {
    QMAKE_CXXFLAGS_RELEASE -= -O2 -O1 -O0 -Os
    QMAKE_CFLAGS_RELEASE -= -O2 -O1 -O0 -Os
    QMAKE_CXXFLAGS_RELEASE *= -O3 -Wall -Werror -Wextra
    QMAKE_CFLAGS_RELEASE *= -O3 -Wall -Werror -Wextra
}

SOURCES += \
    mgjson_qtest.cpp

include(../mgjson.pri)
