#.rst:
# FindTULibs
# -------
#
# Find TULibs installation
#
# Try to find TULibs on UNIX systems. The following values are defined
#
# ::
#
#   TULIBS_FOUND        - True if TULibs is available
#   TULIBS_INCLUDE_DIR  - include directories to use TULibs
#
# and also the following more fine grained variables:
#
# ::
#
#   TUTOOLSPP_INCLUDE_PATH,	TUTOOLSPP_LIB,	TUTOOLSPP_FOUND
#   TUVPP_INCLUDE_PATH,		TUVPP_LIB,	TUVPP_FOUND
#   TUIIDCPP_INCLUDE_PATH,	TUIIDCPP_LIB,	TUIIDCPP_FOUND
#   TUV4L2PP_INCLUDE_PATH,	TUV4L2PP_LIB,	TUV4L2PP_FOUND

if (UNIX)
  set(TULIBS_INC_SEARCH_PATH
      /usr/include
      /usr/local/include
      /usr/local/openrtp/include)

  set(TULIBS_LIB_SEARCH_PATH
      /usr/lib
      /usr/local/lib
      /usr/local/openrtp/lib)

  set(TULIBS_INCLUDE_DIR)

  find_path(TUTOOLSPP_INCLUDE_PATH TU/algorithm.h ${TULIBS_INC_SEARCH_PATH})
  find_library(TUTOOLSPP_LIB	   TUTools++	  ${TULIBS_LIB_SEARCH_PATH})

  if(TUTOOLSPP_LIB AND TUTOOLSPP_INCLUDE_PATH)
    set(TUTOOLSPP_FOUND TRUE)
    set(TULIBS_INCLUDE_DIR ${TULIBS_INCLUDE_DIR} ${TUTOOLSPP_INCLUDE_PATH})
  endif()

  find_path(TUVPP_INCLUDE_PATH	TU/v/TUv++.h	${TULIBS_INC_SEARCH_PATH})
  find_library(TUVPP_LIB	TUv++		${TULIBS_LIB_SEARCH_PATH})

  if(TUVPP_LIB AND TUVPP_INCLUDE_PATH)
    set(TUVPP_FOUND TRUE)
    set(TULIBS_INCLUDE_DIR ${TULIBS_INCLUDE_DIR} ${TUVPP_INCLUDE_PATH})
  endif()

  find_path(TUIIDCPP_INCLUDE_PATH TU/IIDC++.h	${TULIBS_INC_SEARCH_PATH})
  find_library(TUIIDCPP_LIB	  TUIIDC++	${TULIBS_LIB_SEARCH_PATH})

  if(TUIIDCPP_LIB AND TUIIDCPP_INCLUDE_PATH)
    set(TUIIDCPP_FOUND TRUE)
    set(TULIBS_INCLUDE_DIR ${TULIBS_INCLUDE_DIR} ${TUIIDCPP_INCLUDE_PATH})
  endif()

  find_path(TUV4L2PP_INCLUDE_PATH TU/V4L2++.h	${TULIBS_INC_SEARCH_PATH})
  find_library(TUV4L2PP_LIB	  TUV4L2++	${TULIBS_LIB_SEARCH_PATH})

  if(TUV4L2PP_LIB AND TUV4L2PP_INCLUDE_PATH)
    set(TUV4L2PP_FOUND TRUE)
    set(TULIBS_INCLUDE_DIR ${TULIBS_INCLUDE_DIR} ${TUV4L2PP_INCLUDE_PATH})
  endif()

  if(TUBLIS_INCLUDE_DIR)
    list(REMOVE_DUPLICATES TULIBS_INCLUDE_DIR)
  endif()

  if(TUTOOLSPP_FOUND)
    set(TULIBS_FOUND)
  endif()

  if(NOT TULIBS_FOUND)
    if(TULIBS_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find TULibs")
    endif()
  endif()
endif ()
