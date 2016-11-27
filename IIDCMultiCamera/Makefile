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

INCDIRS		= -I. -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG -DHAVE_LIBTUTOOLS__ -DDEBUG
CFLAGS		= -g
NVCCFLAGS	= -g
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE3
endif

LIBS		= -lTUIIDC++ -lTUTools++ \
		  -lraw1394 -lusb-1.0 -lboost_serialization
#ifneq ($(findstring darwin,$(OSTYPE)),)
#  LIBS	       += -framework IOKit -framework CoreFoundation -framework CoreServices
#endif

LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(shell pkg-config --variable=plugindir choreonoid)/rtc
INCDIR		= $(PREFIX)/include

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= /usr/local/include/TU/Array++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Heap.h \
		/usr/local/include/TU/IIDC++.h \
		/usr/local/include/TU/IIDCCameraUtility.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/algorithm.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/pair.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/types.h
HDRS		= CmdDef.h \
		CmdSVC_impl.h \
		MultiCameraRTC.h
SRCS		= CmdDynSK.cc \
		CmdSK.cc \
		IIDCCmdSVC_impl.cc \
		IIDCMultiCameraComp.cc \
		IIDCMultiCameraRTC.cc \
		ImgDynSK.cc \
		ImgSK.cc
OBJS		= CmdDynSK.o \
		CmdSK.o \
		IIDCCmdSVC_impl.o \
		IIDCMultiCameraComp.o \
		IIDCMultiCameraRTC.o \
		ImgDynSK.o \
		ImgSK.o

include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
#include $(PROJECT)/lib/cnoid.mk	# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
CmdDynSK.o: Cmd.hh
CmdSK.o: Cmd.hh
IIDCCmdSVC_impl.o: /usr/local/include/TU/IIDCCameraUtility.h \
	/usr/local/include/TU/IIDC++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/pair.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/algorithm.h \
	/usr/local/include/TU/Heap.h MultiCameraRTC.h Img.hh CmdSVC_impl.h \
	Cmd.hh CmdDef.h
IIDCMultiCameraRTC.o: /usr/local/include/TU/IIDCCameraUtility.h \
	/usr/local/include/TU/IIDC++.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/functional.h /usr/local/include/TU/pair.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/algorithm.h \
	/usr/local/include/TU/Heap.h MultiCameraRTC.h Img.hh CmdSVC_impl.h \
	Cmd.hh CmdDef.h
ImgDynSK.o: Img.hh
ImgSK.o: Img.hh
