#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
PROGRAM		= $(shell basename $(PWD))Comp
LIBRARY		= $(shell basename $(PWD))RTC

VPATH		= ../TU/idl

IDLS		= MultiImage.idl Cmd.idl
MOCHDRS		=

INCDIRS		= -I. -I../SADStereo -I../include \
		  -I$(PREFIX)/include
CPPFLAGS	= -DNDEBUG -DDEBUG -DUSE_TBB
CFLAGS		= -O3
NVCCFLAGS	= -O
ifneq ($(findstring icpc,$(CXX)),)
  CFLAGS	= -O3 #-xAVX
  NVCCFLAGS	= -O			# must < -O2
  CPPFLAGS     += -DSSE4 #-DAVX
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
EXTHDRS		= ../SADStereo/Cmd.hh \
		../SADStereo/CmdSVC_impl.h \
		../SADStereo/MultiImage.hh \
		../SADStereo/StereoRTC.h \
		../include/CmdDef.h \
		/usr/local/include/TU/Array++.h \
		/usr/local/include/TU/BoxFilter.h \
		/usr/local/include/TU/Camera++.h \
		/usr/local/include/TU/Filter2.h \
		/usr/local/include/TU/GFStereo.h \
		/usr/local/include/TU/Geometry++.h \
		/usr/local/include/TU/Image++.h \
		/usr/local/include/TU/Manip.h \
		/usr/local/include/TU/Minimize.h \
		/usr/local/include/TU/Profiler.h \
		/usr/local/include/TU/Rectify.h \
		/usr/local/include/TU/StereoBase.h \
		/usr/local/include/TU/Vector++.h \
		/usr/local/include/TU/Warp.h \
		/usr/local/include/TU/algorithm.h \
		/usr/local/include/TU/functional.h \
		/usr/local/include/TU/iterator.h \
		/usr/local/include/TU/pair.h \
		/usr/local/include/TU/range.h \
		/usr/local/include/TU/simd/Array++.h \
		/usr/local/include/TU/simd/allocator.h \
		/usr/local/include/TU/simd/arithmetic.h \
		/usr/local/include/TU/simd/arm/arch.h \
		/usr/local/include/TU/simd/arm/arithmetic.h \
		/usr/local/include/TU/simd/arm/bit_shift.h \
		/usr/local/include/TU/simd/arm/cast.h \
		/usr/local/include/TU/simd/arm/compare.h \
		/usr/local/include/TU/simd/arm/cvt.h \
		/usr/local/include/TU/simd/arm/dup.h \
		/usr/local/include/TU/simd/arm/insert_extract.h \
		/usr/local/include/TU/simd/arm/load_store.h \
		/usr/local/include/TU/simd/arm/logical.h \
		/usr/local/include/TU/simd/arm/lookup.h \
		/usr/local/include/TU/simd/arm/select.h \
		/usr/local/include/TU/simd/arm/shift.h \
		/usr/local/include/TU/simd/arm/type_traits.h \
		/usr/local/include/TU/simd/arm/vec.h \
		/usr/local/include/TU/simd/arm/zero.h \
		/usr/local/include/TU/simd/bit_shift.h \
		/usr/local/include/TU/simd/cast.h \
		/usr/local/include/TU/simd/compare.h \
		/usr/local/include/TU/simd/config.h \
		/usr/local/include/TU/simd/cvt.h \
		/usr/local/include/TU/simd/cvtdown_iterator.h \
		/usr/local/include/TU/simd/cvtup_iterator.h \
		/usr/local/include/TU/simd/dup.h \
		/usr/local/include/TU/simd/insert_extract.h \
		/usr/local/include/TU/simd/iterator_wrapper.h \
		/usr/local/include/TU/simd/load_store.h \
		/usr/local/include/TU/simd/load_store_iterator.h \
		/usr/local/include/TU/simd/logical.h \
		/usr/local/include/TU/simd/lookup.h \
		/usr/local/include/TU/simd/map_iterator.h \
		/usr/local/include/TU/simd/misc.h \
		/usr/local/include/TU/simd/select.h \
		/usr/local/include/TU/simd/shift.h \
		/usr/local/include/TU/simd/shift_iterator.h \
		/usr/local/include/TU/simd/simd.h \
		/usr/local/include/TU/simd/transform.h \
		/usr/local/include/TU/simd/type_traits.h \
		/usr/local/include/TU/simd/vec.h \
		/usr/local/include/TU/simd/x86/arch.h \
		/usr/local/include/TU/simd/x86/arithmetic.h \
		/usr/local/include/TU/simd/x86/bit_shift.h \
		/usr/local/include/TU/simd/x86/cast.h \
		/usr/local/include/TU/simd/x86/compare.h \
		/usr/local/include/TU/simd/x86/cvt.h \
		/usr/local/include/TU/simd/x86/dup.h \
		/usr/local/include/TU/simd/x86/insert_extract.h \
		/usr/local/include/TU/simd/x86/load_store.h \
		/usr/local/include/TU/simd/x86/logical.h \
		/usr/local/include/TU/simd/x86/logical_base.h \
		/usr/local/include/TU/simd/x86/lookup.h \
		/usr/local/include/TU/simd/x86/select.h \
		/usr/local/include/TU/simd/x86/shift.h \
		/usr/local/include/TU/simd/x86/shuffle.h \
		/usr/local/include/TU/simd/x86/svml.h \
		/usr/local/include/TU/simd/x86/type_traits.h \
		/usr/local/include/TU/simd/x86/unpack.h \
		/usr/local/include/TU/simd/x86/vec.h \
		/usr/local/include/TU/simd/x86/zero.h \
		/usr/local/include/TU/simd/zero.h \
		/usr/local/include/TU/tuple.h \
		/usr/local/include/TU/type_traits.h
HDRS		=
SRCS		= GFStereoComp.cc \
		GFStereoRTC.cc
OBJS		= GFStereoComp.o \
		GFStereoRTC.o

include $(PROJECT)/lib/rtc.mk		# IDLHDRS, IDLSRCS, CPPFLAGS, OBJS, LIBS
#include $(PROJECT)/lib/qt.mk		# MOCSRCS, OBJS
#include $(PROJECT)/lib/cnoid.mk	# CPPFLAGS, LIBS, LIBDIR
#include $(PROJECT)/lib/lib.mk		# PUBHDRS TARGHDRS
include $(PROJECT)/lib/common.mk
###
GFStereoRTC.o: ../SADStereo/StereoRTC.h ../SADStereo/MultiImage.hh \
	/usr/local/include/TU/Rectify.h /usr/local/include/TU/Warp.h \
	/usr/local/include/TU/simd/Array++.h /usr/local/include/TU/Array++.h \
	/usr/local/include/TU/range.h /usr/local/include/TU/iterator.h \
	/usr/local/include/TU/tuple.h /usr/local/include/TU/type_traits.h \
	/usr/local/include/TU/algorithm.h /usr/local/include/TU/simd/simd.h \
	/usr/local/include/TU/simd/config.h /usr/local/include/TU/simd/vec.h \
	/usr/local/include/TU/simd/type_traits.h \
	/usr/local/include/TU/simd/x86/type_traits.h \
	/usr/local/include/TU/simd/arm/type_traits.h \
	/usr/local/include/TU/simd/x86/vec.h \
	/usr/local/include/TU/simd/x86/arch.h \
	/usr/local/include/TU/simd/arm/vec.h \
	/usr/local/include/TU/simd/arm/arch.h \
	/usr/local/include/TU/simd/allocator.h \
	/usr/local/include/TU/simd/iterator_wrapper.h \
	/usr/local/include/TU/simd/load_store_iterator.h \
	/usr/local/include/TU/simd/load_store.h \
	/usr/local/include/TU/simd/x86/load_store.h \
	/usr/local/include/TU/simd/arm/load_store.h \
	/usr/local/include/TU/simd/zero.h \
	/usr/local/include/TU/simd/x86/zero.h \
	/usr/local/include/TU/simd/arm/zero.h \
	/usr/local/include/TU/simd/cast.h \
	/usr/local/include/TU/simd/x86/cast.h \
	/usr/local/include/TU/simd/arm/cast.h \
	/usr/local/include/TU/simd/insert_extract.h \
	/usr/local/include/TU/simd/x86/insert_extract.h \
	/usr/local/include/TU/simd/arm/insert_extract.h \
	/usr/local/include/TU/simd/shift.h \
	/usr/local/include/TU/simd/x86/shift.h \
	/usr/local/include/TU/simd/arm/shift.h \
	/usr/local/include/TU/simd/bit_shift.h \
	/usr/local/include/TU/simd/x86/bit_shift.h \
	/usr/local/include/TU/simd/arm/bit_shift.h \
	/usr/local/include/TU/simd/dup.h /usr/local/include/TU/simd/cvt.h \
	/usr/local/include/TU/simd/x86/cvt.h \
	/usr/local/include/TU/simd/x86/unpack.h \
	/usr/local/include/TU/simd/arm/cvt.h \
	/usr/local/include/TU/simd/logical.h \
	/usr/local/include/TU/simd/x86/logical.h \
	/usr/local/include/TU/simd/x86/logical_base.h \
	/usr/local/include/TU/simd/arm/logical.h \
	/usr/local/include/TU/simd/x86/dup.h \
	/usr/local/include/TU/simd/arm/dup.h \
	/usr/local/include/TU/simd/compare.h \
	/usr/local/include/TU/simd/x86/compare.h \
	/usr/local/include/TU/simd/arm/compare.h \
	/usr/local/include/TU/simd/select.h \
	/usr/local/include/TU/simd/x86/select.h \
	/usr/local/include/TU/simd/arm/select.h \
	/usr/local/include/TU/simd/arithmetic.h \
	/usr/local/include/TU/simd/x86/arithmetic.h \
	/usr/local/include/TU/simd/arm/arithmetic.h \
	/usr/local/include/TU/simd/misc.h \
	/usr/local/include/TU/simd/x86/shuffle.h \
	/usr/local/include/TU/simd/x86/svml.h \
	/usr/local/include/TU/simd/transform.h \
	/usr/local/include/TU/functional.h \
	/usr/local/include/TU/simd/lookup.h \
	/usr/local/include/TU/simd/x86/lookup.h \
	/usr/local/include/TU/simd/arm/lookup.h \
	/usr/local/include/TU/simd/cvtdown_iterator.h \
	/usr/local/include/TU/simd/cvtup_iterator.h \
	/usr/local/include/TU/simd/shift_iterator.h \
	/usr/local/include/TU/simd/map_iterator.h \
	/usr/local/include/TU/Image++.h /usr/local/include/TU/pair.h \
	/usr/local/include/TU/Manip.h /usr/local/include/TU/Camera++.h \
	/usr/local/include/TU/Geometry++.h /usr/local/include/TU/Minimize.h \
	/usr/local/include/TU/Vector++.h ../SADStereo/CmdSVC_impl.h \
	../SADStereo/Cmd.hh ../include/CmdDef.h \
	/usr/local/include/TU/GFStereo.h /usr/local/include/TU/StereoBase.h \
	/usr/local/include/TU/Profiler.h /usr/local/include/TU/BoxFilter.h \
	/usr/local/include/TU/Filter2.h
