cmake_minimum_required(VERSION 3.1)
project(TURTComponents)

enable_language(CXX)
set(CMAKE_CXX_STANDARD		14)
set(CMAKE_CXX_STANDARD_REQUIRED	ON)
set(CMAKE_CXX_EXTENSIONS	OFF)
set(CMAKE_CXX_FLAGS_RELEASE	"-Wall -O3")
set(CMAKE_CXX_FLAGS_DEBUG	"-g")
set(CMAKE_BUILD_TYPE		Release)
add_definitions(-DNDEBUG)

include(FindPkgConfig)
include(CheckIncludeFiles)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

find_package(Boost REQUIRED)
find_package(TULibs REQUIRED)
find_package(OpenRTM REQUIRED)

pkg_check_modules(HRPSYSBASE hrpsys-base)
if(NOT HRPSYSBASE_FOUND)
  message(FATAL_ERROR "hrpsys-base is not found.")
endif()

pkg_check_modules(OMNIORB omniDynamic4)
if(NOT OMNIORB_FOUND)
  message(FATAL_ERROR "omniORB library is not found.")
endif()

add_definitions(${OMNIORB_CFLAGS})

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${TULIBS_INCLUDE_DIR}
    ${OMNIORB_INCLUDE_DIRS}
    ${OPENRTM_INCLUDE_DIRS}
)

link_directories(
    ${OMNIORB_LIBRARY_DIRS}
    ${OPENRTM_LIBRARY_DIRS}
)

#########################
#  RT-components	#
#########################
set(IDL_INCLUDE_DIRS ${OPENRTM_INCLUDE_DIRS})
include(IdlCompile)

#########################
#  Choreonoid plugins	#
#########################
pkg_check_modules(CNOID choreonoid)
if(CNOID_FOUND)
  include_directories(${CNOID_INCLUDE_DIRS})
  include(CnoidTools)
endif()

#########################
#  Subdirectories	#
#########################
if(TUIIDCPP_FOUND)
  add_subdirectory(IIDCCamera)
  add_subdirectory(IIDCMultiCamera)
endif()
if(TUV4L2PP_FOUND)
  add_subdirectory(V4L2Camera)
  add_subdirectory(V4L2MultiCamera)
endif()
add_subdirectory(VideoSynchronizer)

if(TUVPP_FOUND)
  add_subdirectory(ImageViewer)
  add_subdirectory(MultiImageViewer)
endif()

if(CNOID_FOUND)
  add_subdirectory(TUControlPanelPlugin)
  add_subdirectory(TUImageViewerPlugin)
  add_subdirectory(TUMultiImageViewerPlugin)
  add_subdirectory(cnoid_project)
endif()

add_subdirectory(cmake_modules)
