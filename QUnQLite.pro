QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QUnQLite
TEMPLATE = app

SOURCES += \
    UnQLite/unqlite.c \
    qunqlite.cpp \
    main.cpp \
    qunqlitecursor.cpp

HEADERS  += \
    UnQLite/unqlite.h \
    qunqlite.h \
    qunqlitecursor.h \
    dpointer.h

CONFIG += c++11
