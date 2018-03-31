#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

VPATH		= ${OPENRTPHOME}/share/hrpsys/idl

IDLS		= Img.idl
MOCHDRS		=

INCDIRS		= -I. -I../include
CPPFLAGS	= -DNDEBUG -DDEBUG
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

LIBS		=
LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(shell pkg-config --variable=plugindir choreonoid)/rtc
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/rtc/JointReader

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../include/RTCTime.h
HDRS		= SynchronizerRTC.h
SRCS		= VideoSynchronizerRTC.cc \
		main.cc
OBJS		= VideoSynchronizerRTC.o \
		main.o

OTHER_HDRS	= $(HDRS)
OTHER_SRCS	= $(SRCS)

include $(PROJECT)/lib/rtc.mk		# IDLHDRS, IDLSRCS, CPPFLAGS, OBJS, LIBS
#include $(PROJECT)/lib/qt.mk		# MOCSRCS, OBJS
#include $(PROJECT)/lib/cnoid.mk	# CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
include $(PROJECT)/lib/other.mk
###
VideoSynchronizerRTC.o: SynchronizerRTC.h ../include/RTCTime.h
