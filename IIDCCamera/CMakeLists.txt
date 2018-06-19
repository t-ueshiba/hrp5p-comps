cmake_minimum_required(VERSION 2.4)

set(idls Img Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs
    ${HRPSYSBASE_PREFIX}/share/hrpsys/idl
    ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    IIDCCmdSVC_impl.cc
    IIDCCameraRTC.cc
    saveImageHeader.cc)

add_executable(
    IIDCCameraComp
    main.cc
    ${idl_cpp_files}
    ${idl_h_files}
    ${sources})

target_link_libraries(
    IIDCCameraComp
    TUIIDC++
    boost_serialization
    ${OPENRTM_LIBRARIES})

install(TARGETS IIDCCameraComp RUNTIME DESTINATION bin)

if(BUILD_CNOID_PLUGINS)
  add_cnoid_rtc(
      IIDCCameraRTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      IIDCCameraRTC
      TUIIDC++
      boost_serialization
      ${OPENRTM_LIBRARIES})
endif()

