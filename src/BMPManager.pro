#-------------------------------------------------
#
# Project created by QtCreator 2013-10-13T23:05:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BMPManager
TEMPLATE = app


SOURCES += main.cpp\
    BitmapController.cpp \
    ResultStruct.cpp \
    RequestStruct.cpp \
    MainWindow.cpp \
    ExecutorFactory.cpp \
    OpenImageExecutor.cpp \
    BitmapTO.cpp \
    ModifyImageExecutor.cpp \
    ImageOperation.cpp \
    SaveImageExecutor.cpp \
    BitmapFile.cpp

HEADERS  += \
    BitmapFile.h \
    BitmapController.h \
    ResultStruct.h \
    RequestStruct.h \
    MainWindow.h \
    ExecutorFactory.h \
    IExecutor.h \
    OpenImageExecutor.h \
    ImageOperation.h \
    BitmapTO.h \
    ModifyImageExecutor.h \
    SaveImageExecutor.h

FORMS += \
    mainwindow.ui
