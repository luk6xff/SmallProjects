#-------------------------------------------------
#
# Project created by QtCreator 2014-05-13T15:29:47
#
#-------------------------------------------------

QT       += core gui

TARGET = QtVtkStarter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS    +=  -L/usr/lib/ -lvtkCommon\
            -lvtksys -lQVTK -lvtkViews -lvtkWidgets\
            -lvtkInfovis -lvtkRendering -lvtkGraphics\
            -lvtkImaging -lvtkIO -lvtkFiltering\
            -lvtkDICOMParser -lvtkHybrid

INCLUDEPATH += /usr/include/vtk-5.8/
