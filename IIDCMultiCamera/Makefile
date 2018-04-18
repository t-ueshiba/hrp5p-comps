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

INCDIRS		= -I. -I../include -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG #-DDEBUG
CFLAGS		= -O3
NVCCFLAGS	= -O
ifeq ($(shell arch), armv71)
  CPPFLAGS     += -DNEON
else ifeq ($(shell arch), aarch64)
  CPPFLAGS     += -DNEON
else
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS)

LIBS		= -lusb-1.0 -lTUIIDC++ -lTUTools++ \
		  -lraw1394 -lboost_serialization
ifneq ($(findstring darwin,$(OSTYPE)),)
  LIBS		= -lusb-1.0 -lTUIIDC++ -lTUTools++ \
		  -lraw1394 -lboost_serialization
else
  LIBS		= -lTUIIDC++ -lTUTools++ \
		  -lraw1394 -lusb-1.0 -lboost_serialization
endif

LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(shell pkg-config --variable=plugindir choreonoid)/rtc
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/rtc/IIDCMultiCamera

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../include/CameraRTC.h \
		../include/CmdDef.h \
		../include/CmdSVC_impl.h \
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
SRCS		= IIDCCmdSVC_impl.cc \
		IIDCMultiCameraRTC.cc \
		main.cc \
		saveImageHeader.cc
OBJS		= IIDCCmdSVC_impl.o \
		IIDCMultiCameraRTC.o \
		main.o \
		saveImageHeader.o

OTHER_HDRS	= $(HDRS)
OTHER_SRCS	= $(SRCS)

include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
#include $(PROJECT)/lib/cnoid.mk	# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
include $(PROJECT)/lib/other.mk
###
IIDCCmdSVC_impl.o: /usr/local/include/TU/IIDCCameraArray.h \
	/usr/local/include/TU/IIDC++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/pair.h /usr/local/include/TU/type_traits.h \
	/usr/local/include/TU/Manip.h /usr/local/include/TU/Camera++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/range.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/algorithm.h \
	../include/CameraRTC.h ../include/CmdSVC_impl.h ../include/CmdDef.h
IIDCMultiCameraRTC.o: /usr/local/include/TU/IIDCCameraArray.h \
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
