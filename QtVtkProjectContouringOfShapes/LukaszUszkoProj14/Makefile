#############################################################################
# Makefile for building: MyProj14
# Generated by qmake (2.01a) (Qt 4.8.4) on: niedz. cze 22 05:02:29 2014
# Project:  MyProj14.pro
# Template: app
# Command: /usr/share/qt4/bin/qmake -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile MyProj14.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I/usr/include/vtk-5.8 -I. -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib/i386-linux-gnu -L/usr/lib/ -lvtkCommon -lvtksys -lQVTK -lvtkViews -lvtkWidgets -lvtkInfovis -lvtkRendering -lvtkGraphics -lvtkImaging -lvtkIO -lvtkFiltering -lvtkDICOMParser -lvtkHybrid -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/share/qt4/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = main.cpp \
		mainwindow.cpp \
		shape.cpp \
		cone.cpp \
		sphere.cpp \
		quadric.cpp \
		box.cpp \
		cylinder.cpp \
		shapefactory.cpp moc_mainwindow.cpp \
		moc_shape.cpp \
		moc_shapefactory.cpp
OBJECTS       = main.o \
		mainwindow.o \
		shape.o \
		cone.o \
		sphere.o \
		quadric.o \
		box.o \
		cylinder.o \
		shapefactory.o \
		moc_mainwindow.o \
		moc_shape.o \
		moc_shapefactory.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		MyProj14.pro
QMAKE_TARGET  = MyProj14
DESTDIR       = 
TARGET        = MyProj14

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): ui_mainwindow.h ui_settings.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: MyProj14.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/i386-linux-gnu/libQtGui.prl \
		/usr/lib/i386-linux-gnu/libQtCore.prl
	$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile MyProj14.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/i386-linux-gnu/libQtGui.prl:
/usr/lib/i386-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -spec /usr/share/qt4/mkspecs/linux-g++ -o Makefile MyProj14.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/MyProj141.0.0 || $(MKDIR) .tmp/MyProj141.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/MyProj141.0.0/ && $(COPY_FILE) --parents mainwindow.h shape.h cone.h sphere.h quadric.h box.h cylinder.h shapefactory.h .tmp/MyProj141.0.0/ && $(COPY_FILE) --parents main.cpp mainwindow.cpp shape.cpp cone.cpp sphere.cpp quadric.cpp box.cpp cylinder.cpp shapefactory.cpp .tmp/MyProj141.0.0/ && $(COPY_FILE) --parents mainwindow.ui settings.ui .tmp/MyProj141.0.0/ && (cd `dirname .tmp/MyProj141.0.0` && $(TAR) MyProj141.0.0.tar MyProj141.0.0 && $(COMPRESS) MyProj141.0.0.tar) && $(MOVE) `dirname .tmp/MyProj141.0.0`/MyProj141.0.0.tar.gz . && $(DEL_FILE) -r .tmp/MyProj141.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_mainwindow.cpp moc_shape.cpp moc_shapefactory.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwindow.cpp moc_shape.cpp moc_shapefactory.cpp
moc_mainwindow.cpp: shapefactory.h \
		shape.h \
		sphere.h \
		quadric.h \
		cylinder.h \
		cone.h \
		box.h \
		mainwindow.h
	/usr/lib/i386-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) mainwindow.h -o moc_mainwindow.cpp

moc_shape.cpp: shape.h
	/usr/lib/i386-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) shape.h -o moc_shape.cpp

moc_shapefactory.cpp: shape.h \
		sphere.h \
		quadric.h \
		cylinder.h \
		cone.h \
		box.h \
		shapefactory.h
	/usr/lib/i386-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) shapefactory.h -o moc_shapefactory.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: ui_mainwindow.h ui_settings.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h ui_settings.h
ui_mainwindow.h: mainwindow.ui
	/usr/lib/i386-linux-gnu/qt4/bin/uic mainwindow.ui -o ui_mainwindow.h

ui_settings.h: settings.ui
	/usr/lib/i386-linux-gnu/qt4/bin/uic settings.ui -o ui_settings.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_uic_clean 

####### Compile

main.o: main.cpp mainwindow.h \
		shapefactory.h \
		shape.h \
		sphere.h \
		quadric.h \
		cylinder.h \
		cone.h \
		box.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

mainwindow.o: mainwindow.cpp mainwindow.h \
		shapefactory.h \
		shape.h \
		sphere.h \
		quadric.h \
		cylinder.h \
		cone.h \
		box.h \
		ui_mainwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o mainwindow.o mainwindow.cpp

shape.o: shape.cpp shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o shape.o shape.cpp

cone.o: cone.cpp cone.h \
		shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cone.o cone.cpp

sphere.o: sphere.cpp sphere.h \
		shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o sphere.o sphere.cpp

quadric.o: quadric.cpp quadric.h \
		shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o quadric.o quadric.cpp

box.o: box.cpp box.h \
		shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o box.o box.cpp

cylinder.o: cylinder.cpp cylinder.h \
		shape.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o cylinder.o cylinder.cpp

shapefactory.o: shapefactory.cpp shapefactory.h \
		shape.h \
		sphere.h \
		quadric.h \
		cylinder.h \
		cone.h \
		box.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o shapefactory.o shapefactory.cpp

moc_mainwindow.o: moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_mainwindow.o moc_mainwindow.cpp

moc_shape.o: moc_shape.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_shape.o moc_shape.cpp

moc_shapefactory.o: moc_shapefactory.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_shapefactory.o moc_shapefactory.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

