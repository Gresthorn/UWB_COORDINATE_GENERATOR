#-------------------------------------------------
#
# Project created by QtCreator 2014-09-25T22:30:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalGenerator
TEMPLATE = app


RESOURCES += \
    pictures.qrc

QT += \
    widgets \
    printsupport

FORMS += \
    mainwindow.ui \
    aboutDialog.ui \
    noisedistributionselectdialog.ui \
    setquantizationdialog.ui

HEADERS += \
    dialogs.h \
    mainwindow.h \
    monitoredareascenesubclass.h \
    radaritem.h \
    routemath.h \
    structures.h \
    transformation.h \
    qcustomplot.h \
    routeanimation.h \
    ui_aboutDialog.h \
    aboutDialog.h \
    noisedistributionselectdialog.h \
    setquantizationdialog.h

SOURCES += \
    dialogs.cpp \
    main.cpp \
    mainwindow.cpp \
    monitoredareascenesubclass.cpp \
    radaritem.cpp \
    routemath.cpp \
    transformation.cpp \
    qcustomplot.cpp \
    routeanimation.cpp \
    aboutDialog.cpp \
    noisedistributionselectdialog.cpp \
    setquantizationdialog.cpp

RC_FILE = iconrc.rc

CONFIG += c++11
