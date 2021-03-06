#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
#PROGRAM		= $(shell basename $(PWD))
LIBRARY		= libCnoid$(shell basename $(PWD))

VPATH		= ../include/TU/idl

IDLS		= Cmd.idl
MOCHDRS		= ControlPanelView.h LabelCmd_.h ButtonCmd_.h \
		  ToggleButtonCmd_.h SliderCmd_.h GroupBoxCmd_.h Menu_.h \
		  Dialog_.h

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

LIBS		= -lboost_serialization
LINKER		= $(CXX)

BINDIR		= $(PREFIX)/bin
LIBDIR		= $(PREFIX)/lib
INCDIR		= $(PREFIX)/include

OTHER_DIR	= $(HOME)/projects/HRP-5P/hrp5p-calib/src/cnoid/TUControlPanelPlugin

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .cc:sC .cpp:sC .cu:sC
EXTHDRS		= ../include/CmdDef.h
HDRS		= ButtonCmd_.h \
		ControlPanelRTC.h \
		ControlPanelView.h \
		Dialog_.h \
		GroupBoxCmd_.h \
		LabelCmd_.h \
		Menu_.h \
		SliderCmd_.h \
		ToggleButtonCmd_.h
SRCS		= ButtonCmd.cc \
		ControlPanelPlugin.cc \
		ControlPanelRTC.cc \
		ControlPanelView.cc \
		Dialog.cc \
		GroupBoxCmd.cc \
		LabelCmd.cc \
		Menu.cc \
		SliderCmd.cc \
		ToggleButtonCmd.cc \
		createCmd.cc
OBJS		= ButtonCmd.o \
		ControlPanelPlugin.o \
		ControlPanelRTC.o \
		ControlPanelView.o \
		Dialog.o \
		GroupBoxCmd.o \
		LabelCmd.o \
		Menu.o \
		SliderCmd.o \
		ToggleButtonCmd.o \
		createCmd.o

OTHER_HDRS	= $(HDRS)
OTHER_SRCS	= $(SRCS)

include $(PROJECT)/lib/qt.mk
include $(PROJECT)/lib/rtc.mk		# modified: CPPFLAGS, LIBS
include $(PROJECT)/lib/cnoid.mk		# modified: CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# added:    PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
include $(PROJECT)/lib/other.mk
###
ButtonCmd.o: ButtonCmd_.h Dialog_.h ../include/CmdDef.h Menu_.h
ControlPanelPlugin.o: ControlPanelView.h ControlPanelRTC.h \
	../include/CmdDef.h
ControlPanelRTC.o: ControlPanelRTC.h ../include/CmdDef.h
ControlPanelView.o: ControlPanelView.h ControlPanelRTC.h \
	../include/CmdDef.h
Dialog.o: Dialog_.h ../include/CmdDef.h
GroupBoxCmd.o: GroupBoxCmd_.h ../include/CmdDef.h
LabelCmd.o: LabelCmd_.h ../include/CmdDef.h
Menu.o: Menu_.h Dialog_.h ../include/CmdDef.h
SliderCmd.o: SliderCmd_.h ../include/CmdDef.h
ToggleButtonCmd.o: ToggleButtonCmd_.h ../include/CmdDef.h
createCmd.o: LabelCmd_.h ../include/CmdDef.h ButtonCmd_.h Dialog_.h \
	ToggleButtonCmd_.h GroupBoxCmd_.h SliderCmd_.h
