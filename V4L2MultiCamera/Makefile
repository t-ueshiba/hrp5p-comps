#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

VPATH		= ../idl

IDLS		= Img.idl Cmd.idl

INCDIRS		= -I. -I$(PREFIX)/include -I../Ieee1394MultiCamera
CPPFLAGS	= -DNDEBUG -DHAVE_LIBTUTOOLS__ -DDEBUG
CFLAGS		= -g
NVCCFLAGS	= -g
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS) -std=c++0x

LIBS		= -lTUV4L2++ -lTUTools++ -lboost_serialization
ifneq ($(findstring darwin,$(OSTYPE)),)
  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
endif

LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../Ieee1394MultiCamera/Cmd.hh \
		../Ieee1394MultiCamera/CmdDef.h \
		../Ieee1394MultiCamera/CmdSVC_impl.h \
		../Ieee1394MultiCamera/Img.hh \
		../Ieee1394MultiCamera/MultiCamera.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/V4L2++.h \
		/usr/local/include/TU/V4L2CameraArray.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/io.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/types.h
HDRS		=
SRCS		= V4L2CmdSVC_impl.cc \
		V4L2MultiCamera.cc \
		V4L2MultiCameraComp.cc
OBJS		= V4L2CmdSVC_impl.o \
		V4L2MultiCamera.o \
		V4L2MultiCameraComp.o

include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
#include $(PROJECT)/lib/cnoid.mk	# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
V4L2CmdSVC_impl.o: /usr/local/include/TU/V4L2CameraArray.h \
	/usr/local/include/TU/V4L2++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/io.h \
	../Ieee1394MultiCamera/CmdSVC_impl.h ../Ieee1394MultiCamera/Cmd.hh \
	../Ieee1394MultiCamera/CmdDef.h
V4L2MultiCamera.o: /usr/local/include/TU/V4L2CameraArray.h \
	/usr/local/include/TU/V4L2++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/io.h \
	../Ieee1394MultiCamera/MultiCamera.h ../Ieee1394MultiCamera/Img.hh \
	../Ieee1394MultiCamera/CmdSVC_impl.h ../Ieee1394MultiCamera/Cmd.hh \
	../Ieee1394MultiCamera/CmdDef.h
V4L2MultiCameraComp.o: ../Ieee1394MultiCamera/MultiCamera.h \
	../Ieee1394MultiCamera/Img.hh ../Ieee1394MultiCamera/CmdSVC_impl.h \
	../Ieee1394MultiCamera/Cmd.hh ../Ieee1394MultiCamera/CmdDef.h \
	/usr/local/include/TU/types.h
