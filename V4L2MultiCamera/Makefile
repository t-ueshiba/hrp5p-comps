#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

IDLDIR		= ../idl
IDLS		= $(IDLDIR)/Img.idl

INCDIRS		= -I. -I$(PREFIX)/include -I../Ieee1394MultiCamera
CPPFLAGS	= -DNDEBUG -DHAVE_LIBTUTOOLS__
CFLAGS		= -g
NVCCFLAGS	= -g
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LIBS		= -lTUV4L2++ -lTUTools++
ifneq ($(findstring darwin,$(OSTYPE)),)
  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
endif

LINKER		= $(CXX)

PROGDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
HDRSDIR		= $(PREFIX)/include

#########################
#  Macros set by mkmf	#
#########################
.SUFFIXES:	.cu .idl .hh .so
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../Ieee1394MultiCamera/MultiCamera.h
HDRS		=
SRCS		= V4L2MultiCamera.cc \
		V4L2MultiCameraComp.cc
OBJS		= V4L2MultiCamera.o \
		V4L2MultiCameraComp.o

include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
#include $(PROJECT)/lib/cnoid.mk	# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
V4L2MultiCamera.o: ../Ieee1394MultiCamera/MultiCamera.h
V4L2MultiCameraComp.o: ../Ieee1394MultiCamera/MultiCamera.h
