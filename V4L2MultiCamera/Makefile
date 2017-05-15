#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

VPATH		= ../TU/idl

IDLS		= Img.idl Cmd.idl

INCDIRS		= -I. -I$(PREFIX)/include -I../IIDCMultiCamera
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

LIBS		= -lTUV4L2++ -lTUTools++ -lboost_serialization
LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(shell pkg-config --variable=plugindir choreonoid)/rtc
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/src/rtc/V4L2MultiCamera

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../IIDCMultiCamera/CmdDef.h \
		../IIDCMultiCamera/CmdSVC_impl.h \
		../IIDCMultiCamera/MultiCameraRTC.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/V4L2++.h \
		/usr/local/include/TU/V4L2CameraArray.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/algorithm.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/pair.h \
		/usr/local/include/TU/range.h \
		/usr/local/include/TU/tuple.h
HDRS		=
SRCS		= V4L2CmdSVC_impl.cc \
		V4L2MultiCameraRTC.cc \
		main.cc \
		saveImageHeader.cc
OBJS		= V4L2CmdSVC_impl.o \
		V4L2MultiCameraRTC.o \
		main.o \
		saveImageHeader.o

OTHER_HDRS	= $(HDRS)
OTHER_SRCS	= $(SRCS)

include $(PROJECT)/lib/rtc.mk		# IDLHDRS, IDLSRCS, CPPFLAGS, OBJS, LIBS
#include $(PROJECT)/lib/qt.mk		# MOCSRCS, OBJS
#include $(PROJECT)/lib/cnoid.mk	# CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
include $(PROJECT)/lib/other.mk
###
V4L2CmdSVC_impl.o: /usr/local/include/TU/V4L2CameraArray.h \
	/usr/local/include/TU/V4L2++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/range.h \
	/usr/local/include/TU/algorithm.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h ../IIDCMultiCamera/MultiCameraRTC.h \
	../IIDCMultiCamera/CmdSVC_impl.h ../IIDCMultiCamera/CmdDef.h
V4L2MultiCameraRTC.o: /usr/local/include/TU/V4L2CameraArray.h \
	/usr/local/include/TU/V4L2++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/Vector++.h \
	/usr/local/include/TU/Array++.h /usr/local/include/TU/range.h \
	/usr/local/include/TU/algorithm.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h ../IIDCMultiCamera/MultiCameraRTC.h \
	../IIDCMultiCamera/CmdSVC_impl.h ../IIDCMultiCamera/CmdDef.h
