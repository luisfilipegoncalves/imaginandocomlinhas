#-------------------------------------------------
#
# Project created by QtCreator 2013-07-21T19:27:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imaginandocomlinhas
TEMPLATE = app


SOURCES += main.cpp\
        ICLMainWindow.cpp \
    ICLController.cpp \
    ICLTagManager.cpp \
    ICLImagesManager.cpp \
    ICLListView.cpp

HEADERS  += ICLMainWindow.h \
    ICLController.h \
    ICLTagManager.h \
    ICLImagesManager.h \
    ICLListView.h

FORMS    += ICLMainWindow.ui
