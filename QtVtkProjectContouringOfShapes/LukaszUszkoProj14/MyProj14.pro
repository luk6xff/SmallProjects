#-------------------------------------------------
#
# Project created by QtCreator 2014-05-13T15:29:47
#
#-------------------------------------------------

QT       += core gui

TARGET = MyProj14
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    shape.cpp \
    cone.cpp \
    sphere.cpp \
    quadric.cpp \
    box.cpp \
    cylinder.cpp \
    shapefactory.cpp

HEADERS  += mainwindow.h \
    shape.h \
    cone.h \
    sphere.h \
    quadric.h \
    box.h \
    cylinder.h \
    shapefactory.h

FORMS    += mainwindow.ui \
    settings.ui

LIBS    +=  -L/usr/lib/ -lvtkCommon\
            -lvtksys -lQVTK -lvtkViews -lvtkWidgets\
            -lvtkInfovis -lvtkRendering -lvtkGraphics\
            -lvtkImaging -lvtkIO -lvtkFiltering\
            -lvtkDICOMParser -lvtkHybrid

INCLUDEPATH += /usr/include/vtk-5.8/
