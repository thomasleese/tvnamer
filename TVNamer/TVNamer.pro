QT       += core gui network widgets

TARGET = TVNamer
TEMPLATE = app

SOURCES += main.cpp \
        mainwindow.cpp \
    findshowdialogue.cpp \
    seasonwidget.cpp \
    thetvdb.cpp

HEADERS  += mainwindow.h \
    findshowdialogue.h \
    seasonwidget.h \
    thetvdb.h

FORMS    += mainwindow.ui \
    findshowdialogue.ui \
    seasonwidget.ui
