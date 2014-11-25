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
MOCHDRS		=

INCDIRS		= -I. -I../Ieee1394MultiCamera -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG -DDEBUG -DUSE_TBB
CFLAGS		= -g
NVCCFLAGS	= -g
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE3
endif
CCFLAGS		= $(CFLAGS) -std=c++11

LIBS		= -lTUTools++ -lboost_serialization -ltbb -ltbbmalloc
ifneq ($(findstring darwin,$(OSTYPE)),)
  LIBS	       += -framework IOKit -framework CoreFoundation \
		  -framework CoreServices
endif

LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(shell pkg-config --variable=plugindir choreonoid)/rtc
INCDIR		= $(PREFIX)/include

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../Ieee1394MultiCamera/Cmd.hh \
		../Ieee1394MultiCamera/CmdDef.h \
		../Ieee1394MultiCamera/Img.hh \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/BoxFilter.h \
		/usr/local/include/TU/Camera++.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Profiler.h \
		/usr/local/include/TU/Rectify.h \
		/usr/local/include/TU/SADStereo.h \
		/usr/local/include/TU/SeparableFilter2.h \
		/usr/local/include/TU/StereoBase.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/Warp.h \
		/usr/local/include/TU/algorithm.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/mmInstructions.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/types.h
HDRS		= CmdSVC_impl.h \
		StereoRTC.h
SRCS		= SADStereoComp.cc \
		SADStereoRTC.cc
OBJS		= SADStereoComp.o \
		SADStereoRTC.o

include $(PROJECT)/lib/rtc.mk		# IDLHDRS, IDLSRCS, CPPFLAGS, OBJS, LIBS
#include $(PROJECT)/lib/qt.mk		# MOCSRCS, OBJS
#include $(PROJECT)/lib/cnoid.mk	# CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
SADStereoRTC.o: StereoRTC.h /usr/local/include/TU/Rectify.h \
	/usr/local/include/TU/Warp.h /usr/local/include/TU/Image++.h \
	/usr/local/include/TU/types.h /usr/local/include/TU/Geometry++.h \
	/usr/local/include/TU/Vector++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/iterator.h /usr/local/include/TU/functional.h \
	/usr/local/include/TU/mmInstructions.h /usr/local/include/TU/tuple.h \
	/usr/local/include/TU/Minimize.h /usr/local/include/TU/Camera++.h \
	../Ieee1394MultiCamera/Img.hh CmdSVC_impl.h \
	../Ieee1394MultiCamera/Cmd.hh ../Ieee1394MultiCamera/CmdDef.h \
	/usr/local/include/TU/SADStereo.h /usr/local/include/TU/StereoBase.h \
	/usr/local/include/TU/algorithm.h /usr/local/include/TU/Profiler.h \
	/usr/local/include/TU/BoxFilter.h \
	/usr/local/include/TU/SeparableFilter2.h
