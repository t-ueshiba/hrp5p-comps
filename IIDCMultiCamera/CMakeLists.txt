cmake_minimum_required(VERSION 2.4)

set(idls MultiImage Cmd)

set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    IIDCCmdSVC_impl.cc
    IIDCMultiCameraRTC.cc
    saveImageHeader.cc)

add_executable(
    IIDCMultiCameraComp
    main.cc
    ${idl_cpp_files}
    ${idl_h_files}
    ${sources})

target_link_libraries(
    IIDCMultiCameraComp
    TUIIDC++
    boost_serialization
    ${OPENRTM_LIBRARIES})

install(TARGETS IIDCMultiCameraComp RUNTIME DESTINATION bin)

if(BUILD_CNOID_PLUGINS)
  add_cnoid_rtc(
      IIDCMultiCameraRTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      IIDCMultiCameraRTC
      TUIIDC++
      boost_serialization
      ${OPENRTM_LIBRARIES})
endif()

