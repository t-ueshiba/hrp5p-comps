#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

VPATH		= ${OPENRTPHOME}/share/hrpsys/idl:../include/TU/idl

IDLS		= Img.idl Cmd.idl

INCDIRS		= -I. -I../include -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG #-DDEBUG
CFLAGS		= -g #-O3
NVCCFLAGS	= -O
ifeq ($(shell arch), armv7l)
  CPPFLAGS     += -DNEON
else ifeq ($(shell arch), aarch64)
  CPPFLAGS     += -DNEON
else
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LIBS		= -lTUIIDC++ -lTUTools++ \
		  -lraw1394 -lusb-1.0 -lboost_serialization -lyaml-cpp
LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/rtc/V4L2MultiCamera

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../include/CameraRTC.h \
		../include/CmdDef.h \
		../include/CmdSVC_impl.h \
		../include/RTCTime.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Camera++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/IIDC++.h \
		/usr/local/include/TU/IIDCCameraArray.h \
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
HDRS		=
SRCS		= IIDCCameraRTC.cc \
		IIDCCmdSVC_impl.cc \
		main.cc \
		saveImageHeader.cc
OBJS		= IIDCCameraRTC.o \
		IIDCCmdSVC_impl.o \
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
IIDCCameraRTC.o: /usr/local/include/TU/IIDCCameraArray.h \
	/usr/local/include/TU/IIDC++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/type_traits.h \
	/usr/local/include/TU/Manip.h /usr/local/include/TU/Camera++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/range.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/algorithm.h \
	../include/CameraRTC.h ../include/CmdSVC_impl.h ../include/CmdDef.h \
	../include/RTCTime.h
IIDCCmdSVC_impl.o: /usr/local/include/TU/IIDCCameraArray.h \
	/usr/local/include/TU/IIDC++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/type_traits.h \
	/usr/local/include/TU/Manip.h /usr/local/include/TU/Camera++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/range.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/algorithm.h \
	../include/CameraRTC.h ../include/CmdSVC_impl.h ../include/CmdDef.h
saveImageHeader.o: ../include/CameraRTC.h ../include/CmdSVC_impl.h \
	../include/CmdDef.h
