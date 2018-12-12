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
pkg_check_modules(HRPSYSBASE hrpsys-base)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/corba
    ${Boost_INCLUDE_DIRS}
    ${TULIBS_INCLUDE_DIR})

#########################
#  RT-components	#
#########################
#
# setting up CORBA/OmniORB
#
if(NOT OMNIORB_DIR)
  #pkg_check_modules(OMNIORB omniORB4)
  pkg_check_modules(OMNIORB omniDynamic4)
  if(OMNIORB_FOUND)
    set(OMNIORB_DIR ${OMNIORB_PREFIX})
  endif()
else()
  set(OMNIORB_FOUND TRUE)
  set(OMNIORB_INCLUDE_DIRS ${OMNIORB_DIR}/include)
  set(OMNIORB_LIBRARY_DIRS ${OMNIORB_DIR}/lib)
endif()
  
include_directories(${OMNIORB_INCLUDE_DIRS})
link_directories(${OMNIORB_LIBRARY_DIRS})
add_definitions(${OMNIORB_CFLAGS})
  
set(OMNIORB_DIR ${OMNIORB_DIR} CACHE PATH "The top directory of omniORB")
set(OMNIORB_CFLAGS ${OMNIORB_CFLAGS} CACHE STRING "Compile flags for omniORB")

if(NOT OMNIORB_FOUND)
  message(FATAL_ERROR "CORBA-related modules require the omniORB library but the library is not found.")
endif()

function(idl_compile_cpp out_cpp_files out_header_files subdir corba_src_dirs)
  
  set(corba_dir ${PROJECT_BINARY_DIR}/${subdir})

  set(idl_names ${ARGV})
  list(REMOVE_AT idl_names 0 1 2)
    
  set(idl_flags -C ${corba_dir} -bcxx -Wbh=.hh -Wbs=Sk.cc -Wba -Wbd=DynSk.cc -nf -Wbkeep_inc_path)

  foreach(idl_include_dir ${IDL_INCLUDE_DIRS})
    set(idl_flags ${idl_flags} -I${idl_include_dir})
  endforeach()
    
  file(MAKE_DIRECTORY ${corba_dir})

  #
  # idl compile
  #
  foreach(idl_name ${idl_names})
    foreach(corba_src_dir ${corba_src_dirs})
      if(EXISTS ${corba_src_dir}/${idl_name}.idl)
        add_custom_command(
	  OUTPUT ${corba_dir}/${idl_name}.hh ${corba_dir}/${idl_name}DynSk.cc ${corba_dir}/${idl_name}Sk.cc
	  COMMAND omniidl ${idl_flags} ${corba_src_dir}/${idl_name}.idl
	  DEPENDS ${corba_src_dir}/${idl_name}.idl
	  COMMENT "Generating the C++ stubs and skeletons of ${idl_name}.idl"
	)
	set(idl_files ${idl_files} ${corba_dir}/${idl_name}.idl)
	set(idl_cpp_files
      	    ${idl_cpp_files}
	    ${corba_dir}/${idl_name}DynSk.cc
	    ${corba_dir}/${idl_name}Sk.cc)
	set(idl_h_files
	    ${idl_h_files}
	    ${corba_dir}/${idl_name}.hh)
	break()
      endif()
    endforeach()
  endforeach()
    
  set(${out_cpp_files} ${idl_cpp_files} PARENT_SCOPE)
  set(${out_header_files} ${idl_h_files} PARENT_SCOPE)

  set_source_files_properties(${idl_cpp_files} PROPERTIES GENERATED true COMPILE_FLAGS -DOMNI_UNLOADABLE_STUBS)
    
endfunction()

#
# setting up OpenRTM
#
find_package(OpenRTM)

set(IDL_INCLUDE_DIRS ${OPENRTM_INCLUDE_DIRS})
include_directories(${OPENRTM_INCLUDE_DIRS})
link_directories(${OPENRTM_LIBRARY_DIRS})

#########################
#  Choreonoid plugins	#
#########################
pkg_check_modules(CNOID choreonoid)

if(CNOID_FOUND)

  set(cnoid_uses_qt5 FALSE)
  if(APPLE)
    # CNOID_INCLUDE_DIRS のいずれかの項目に "qt/5" という部分文
    # 字列が含まれているか調べることにより，choreonoid が
    # Qt4/Qt5 のどちらを使っているかを判定する．
    foreach(cnoid_include_dir ${CNOID_INCLUDE_DIRS})
      string(REGEX MATCH "qt/5" matched ${cnoid_include_dir})
      if(${matched} MATCHES "qt/5")
	set(cnoid_uses_qt5 TRUE)
      endif()
    endforeach()

    # CNOID_LDFLAGS の各項目を共有ライブラリ生成のためのリンカフ
    # ラグにコピーする．
    foreach(cnoid_ldflag ${CNOID_LDFLAGS})
      set(CMAKE_SHARED_LINKER_FLAGS
          "${CMAKE_SHARED_LINKER_FLAGS} ${cnoid_ldflag}")
    endforeach()
  else()
    # CNOID_LIBRARIES に Qt5Core が含まれているか調べることにより，
    # choreonoid が Qt4/Qt5 のどちらを使っているかを判定する．
    foreach(cnoid_library ${CNOID_LIBRARIES})
      if(${cnoid_library} MATCHES Qt5Core)
	set(cnoid_uses_qt5 TRUE)
      endif()
    endforeach()
  endif()
  
  # choreonoid が使っている Qt のバージョンに応じたパッケージの
  # 設定を読み込む．
  if(cnoid_uses_qt5)
    find_package(Qt5Core)
    find_package(Qt5Widgets)
    find_package(Qt5OpenGL)
    find_package(Qt5Network)
  else()
    find_package(Qt4 4.7.0 REQUIRED)
  endif()

  # MOC ヘッダから moc によってソースファイルを生成するマクロ.
  if(cnoid_uses_qt5)
    macro(qt_wrap_cpp moc_sources)
      QT5_WRAP_CPP(moc_sources ${ARGN})
    endmacro()
  else()
    macro(qt_wrap_cpp moc_sources)
      QT4_WRAP_CPP(moc_sources ${ARGN})
    endmacro()
  endif()

  # choreonoid の major version を調べ，plugin の場所を指定
  # する sub directory 名を決定する．
  string(REGEX REPLACE ".[0-9]*$" "" CNOID_MAJOR_VERSION ${CNOID_VERSION})
  set(CNOID_SUBDIR choreonoid-${CNOID_MAJOR_VERSION})
  set(CNOID_PLUGIN_SUBDIR lib/${CNOID_SUBDIR})

  # plugin 生成コマンド
  function(add_cnoid_plugin target)
    add_library(${target} SHARED ${ARGN})

    set_target_properties(${target} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}
      ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR})
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR} CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel
      LIBRARY DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR} CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel)
  endfunction()

  # RTC 生成コマンド
  function(add_cnoid_rtc target)
    add_library(${target} SHARED ${ARGN})

    set_target_properties(${target} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}/rtc
      PREFIX ""
      SUFFIX ".so")
    install(TARGETS ${target}
      RUNTIME DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel
      LIBRARY DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel)
  endfunction()

  include_directories(${CNOID_INCLUDE_DIRS})

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
endif()
