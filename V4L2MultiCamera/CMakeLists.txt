cmake_minimum_required(VERSION 2.4)
project(V4L2MultiCamera)

set(idls MultiImage Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba	"${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)

set(sources
    V4L2CmdSVC_impl.cc
    V4L2MultiCameraRTC.cc
    saveImageHeader.cc)

add_executable(
    ${PROJECT_NAME}Comp
    main.cc
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ${PROJECT_NAME}Comp
    ${TUV4L2PP_LIB}
    ${OPENRTM_LIBRARIES}
    boost_serialization)

install(TARGETS ${PROJECT_NAME}Comp RUNTIME DESTINATION bin)

if(CNOID_FOUND)
  add_cnoid_rtc(
      ${PROJECT_NAME}RTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      ${PROJECT_NAME}RTC
      ${TUV4L2PP_LIB}
      ${OPENRTM_LIBRARIES}
      boost_serialization)
endif()

