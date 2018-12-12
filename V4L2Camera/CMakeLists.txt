cmake_minimum_required(VERSION 2.4)

pkg_check_modules(YAML-CPP REQUIRED yaml-cpp)

include_directories(${YAML-CPP_INCLUDE_DIRS})

set(idls Img Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs
    ${HRPSYSBASE_PREFIX}/share/hrpsys/idl
    ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    V4L2CmdSVC_impl.cc
    V4L2CameraRTC.cc
    saveImageHeader.cc)

add_executable(
    V4L2CameraComp
    main.cc
    ${idl_cpp_files}
    ${idl_h_files}
    ${sources})

target_link_libraries(
    V4L2CameraComp
    ${TUV4L2PP_LIB}
    ${OPENRTM_LIBRARIES}
    ${YAML-CPP_LIBRARIES}
    boost_serialization)

install(TARGETS V4L2CameraComp RUNTIME DESTINATION bin)

if(CNOID_FOUND)
  add_cnoid_rtc(
      V4L2CameraRTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      V4L2CameraRTC
      ${TUV4L2PP_LIB}
      ${OPENRTM_LIBRARIES}
      boost_serialization)
endif()

