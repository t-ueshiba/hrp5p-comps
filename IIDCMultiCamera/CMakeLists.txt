cmake_minimum_required(VERSION 3.0)
project(IIDCMultiCamera)

set(idls MultiImage Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/include/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba "${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)

set(sources
    IIDCCmdSVC_impl.cc
    IIDCMultiCameraRTC.cc
    saveImageHeader.cc)

add_executable(
    ${PROJECT_NAME}Comp
    main.cc
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ${PROJECT_NAME}Comp
    ${TUIIDCPP_LIB}
    ${OPENRTM_LIBRARIES}
    boost_serialization)

install(TARGETS ${PROJECT_NAME}Comp RUNTIME DESTINATION bin)

if(CNOID_FOUND)
  add_cnoid_rtc(
      ${PROJECT_NAME}RTC
      IIDCMultiCameraRTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      ${PROJECT_NAME}RTC
      ${TUIIDCPP_LIB}
      ${OPENRTM_LIBRARIES}
      boost_serialization)
endif()
