#.rst:
# FindX11
# -------
#
# Find X11 installation
#
# Try to find X11 on UNIX systems. The following values are defined
#
# ::
#
#   X11_FOUND        - True if X11 is available
#   X11_INCLUDE_DIR  - include directories to use X11
#   X11_LIBRARIES    - link against these to use X11
#
# and also the following more fine grained variables:
#
# ::
#
#   X11_ICE_INCLUDE_PATH,          X11_ICE_LIB,        X11_ICE_FOUND
#   X11_SM_INCLUDE_PATH,           X11_SM_LIB,         X11_SM_FOUND
#   X11_X11_INCLUDE_PATH,          X11_X11_LIB
#   X11_Xaccessrules_INCLUDE_PATH,                     X11_Xaccess_FOUND
#   X11_Xaccessstr_INCLUDE_PATH,                       X11_Xaccess_FOUND
#   X11_Xau_INCLUDE_PATH,          X11_Xau_LIB,        X11_Xau_FOUND
#   X11_Xcomposite_INCLUDE_PATH,   X11_Xcomposite_LIB, X11_Xcomposite_FOUND
#   X11_Xcursor_INCLUDE_PATH,      X11_Xcursor_LIB,    X11_Xcursor_FOUND
#   X11_Xdamage_INCLUDE_PATH,      X11_Xdamage_LIB,    X11_Xdamage_FOUND
#   X11_Xdmcp_INCLUDE_PATH,        X11_Xdmcp_LIB,      X11_Xdmcp_FOUND
#   X11_Xext_LIB,       X11_Xext_FOUND
#   X11_dpms_INCLUDE_PATH,         (in X11_Xext_LIB),  X11_dpms_FOUND
#   X11_XShm_INCLUDE_PATH,         (in X11_Xext_LIB),  X11_XShm_FOUND
#   X11_Xshape_INCLUDE_PATH,       (in X11_Xext_LIB),  X11_Xshape_FOUND
#   X11_xf86misc_INCLUDE_PATH,     X11_Xxf86misc_LIB,  X11_xf86misc_FOUND
#   X11_xf86vmode_INCLUDE_PATH,    X11_Xxf86vm_LIB     X11_xf86vmode_FOUND
#   X11_Xfixes_INCLUDE_PATH,       X11_Xfixes_LIB,     X11_Xfixes_FOUND
#   X11_Xft_INCLUDE_PATH,          X11_Xft_LIB,        X11_Xft_FOUND
#   X11_Xi_INCLUDE_PATH,           X11_Xi_LIB,         X11_Xi_FOUND
#   X11_Xinerama_INCLUDE_PATH,     X11_Xinerama_LIB,   X11_Xinerama_FOUND
#   X11_Xinput_INCLUDE_PATH,       X11_Xinput_LIB,     X11_Xinput_FOUND
#   X11_Xkb_INCLUDE_PATH,                              X11_Xkb_FOUND
#   X11_Xkblib_INCLUDE_PATH,                           X11_Xkb_FOUND
#   X11_Xkbfile_INCLUDE_PATH,      X11_Xkbfile_LIB,    X11_Xkbfile_FOUND
#   X11_Xmu_INCLUDE_PATH,          X11_Xmu_LIB,        X11_Xmu_FOUND
#   X11_Xpm_INCLUDE_PATH,          X11_Xpm_LIB,        X11_Xpm_FOUND
#   X11_XTest_INCLUDE_PATH,        X11_XTest_LIB,      X11_XTest_FOUND
#   X11_Xrandr_INCLUDE_PATH,       X11_Xrandr_LIB,     X11_Xrandr_FOUND
#   X11_Xrender_INCLUDE_PATH,      X11_Xrender_LIB,    X11_Xrender_FOUND
#   X11_Xscreensaver_INCLUDE_PATH, X11_Xscreensaver_LIB, X11_Xscreensaver_FOUND
#   X11_Xt_INCLUDE_PATH,           X11_Xt_LIB,         X11_Xt_FOUND
#   X11_Xutil_INCLUDE_PATH,                            X11_Xutil_FOUND
#   X11_Xv_INCLUDE_PATH,           X11_Xv_LIB,         X11_Xv_FOUND
#   X11_XSync_INCLUDE_PATH,        (in X11_Xext_LIB),  X11_XSync_FOUND

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if (UNIX)
  set(X11_INC_SEARCH_PATH
    /usr/pkg/xorg/include
    /usr/X11R6/include
    /usr/X11R7/include
    /usr/include/X11
    /usr/openwin/include
    /usr/openwin/share/include
    /opt/graphics/OpenGL/include
    /opt/X11/include
  )

  set(X11_LIB_SEARCH_PATH
    /usr/pkg/xorg/lib
    /usr/X11R6/lib
    /usr/X11R7/lib
    /usr/openwin/lib
    /opt/X11/lib
  )

  find_path(X11_Xaw3d_INCLUDE_PATH X11/Xaw3d/XawInit.h ${X11_INC_SEARCH_PATH})
  find_library(X11_Xaw3d_LIB Xaw3d ${X11_LIB_SEARCH_PATH})

  if(X11_Xaw3d_LIB AND X11_Xaw3d_INCLUDE_PATH)
    set(X11_Xaw3d_FOUND TRUE)
  endif()

  if (X11_Xaw3d_INCLUDE_PATH)
    message(STATUS "Looking for libXaw3d - found")
  else (X11_Xaw3d_INCLUDE_PATH)
    message(FATAL_ERROR "Looking for libXaw3d - not found")
  endif (X11_Xaw3d_INCLUDE_PATH)
endif ()

