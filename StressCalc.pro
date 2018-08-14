#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T09:47:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(printsupport): QT += printsupport

TARGET = StressCalc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    soilmat.cpp \
    qcustomplot.cpp \
    inputspreadsheet.cpp

HEADERS  += mainwindow.h \
    soilmat.h \
    qcustomplot.h \
    inputspreadsheet.h
