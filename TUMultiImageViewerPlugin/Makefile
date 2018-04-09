#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
#PROGRAM		= $(shell basename $(PWD))
LIBRARY		= libCnoid$(shell basename $(PWD))

VPATH		= $(PREFIX)/include/TU/idl

IDLS		= Img.idl
MOCHDRS		= ImageView.h

#CXX		= c++

INCDIRS		= -I. -I../include -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG #-DDEBUG
CFLAGS		= -O3
NVCCFLAGS	= -O
ifeq ($(shell arch), armv7l)
  CPPFLAGS     += -DNEON
else ifeq ($(shell arch), aarch64)
  CPPFLAGS     += -DNEON
else
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LIBS		= -lTUTools++
LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/src/cnoid/TUImageViewerPlugin

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../include/ImageViewer.h \
		../include/ImageViewerRTC.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Camera++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Manip.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/algorithm.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/pair.h \
		/usr/local/include/TU/range.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/type_traits.h
HDRS		= ImageView.h
SRCS		= ImageView.cc \
		ImageViewerRTC.cc \
		MultiImageViewerPlugin.cc \
		setImage.cc
OBJS		= ImageView.o \
		ImageViewerRTC.o \
		MultiImageViewerPlugin.o \
		setImage.o

OTHER_HDRS	= $(HDRS)
OTHER_SRCS	= $(SRCS)

include $(PROJECT)/lib/qt.mk
include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
include $(PROJECT)/lib/cnoid.mk		# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
include $(PROJECT)/lib/other.mk
###
ImageView.o: ImageView.h
ImageViewerRTC.o: ../include/ImageViewer.h ../include/ImageViewerRTC.h \
	ImageView.h
MultiImageViewerPlugin.o: ../include/ImageViewer.h \
	../include/ImageViewerRTC.h ImageView.h
setImage.o: ImageView.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/type_traits.h \
	/usr/local/include/TU/Manip.h /usr/local/include/TU/Camera++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/range.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/algorithm.h
