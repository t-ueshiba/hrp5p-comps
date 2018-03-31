#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
#PROGRAM		= $(shell basename $(PWD))
LIBRARY		= libCnoid$(shell basename $(PWD))

VPATH		= $(OPENRTPHOME)/share/hrpsys/idl

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
		../include/ImageViewerRTC.h
HDRS		= ImageView.h
SRCS		= ImageView.cc \
		ImageViewerPlugin.cc \
		ImageViewerRTC.cc \
		setImage.cc
OBJS		= ImageView.o \
		ImageViewerPlugin.o \
		ImageViewerRTC.o \
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
ImageViewerPlugin.o: ../include/ImageViewer.h ../include/ImageViewerRTC.h \
	ImageView.h
ImageViewerRTC.o: ../include/ImageViewer.h ../include/ImageViewerRTC.h \
	ImageView.h
setImage.o: ImageView.h
