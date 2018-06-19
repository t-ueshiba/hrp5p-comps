cmake_minimum_required(VERSION 2.4)

set(idls MultiImage Cmd)

set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    V4L2CmdSVC_impl.cc
    V4L2MultiCameraRTC.cc
    saveImageHeader.cc)

add_executable(
    V4L2MultiCameraComp
    main.cc
    ${idl_cpp_files}
    ${idl_h_files}
    ${sources})

target_link_libraries(
    V4L2MultiCameraComp
    TUV4L2++
    boost_serialization
    ${OPENRTM_LIBRARIES})

install(TARGETS V4L2MultiCameraComp RUNTIME DESTINATION bin)

if(CNOID_FOUND)
  add_cnoid_rtc(
      V4L2MultiCameraRTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      V4L2MultiCameraRTC
      TUV4L2++
      boost_serialization
      ${OPENRTM_LIBRARIES})
endif()

