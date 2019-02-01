cmake_minimum_required(VERSION 3.0)
project(IIDCCamera)

set(idls Img Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs
    ${HRPSYSBASE_PREFIX}/share/hrpsys/idl
    ${CMAKE_SOURCE_DIR}/include/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba "${corba_src_dirs}" ${idls})

pkg_check_modules(YAML-CPP REQUIRED yaml-cpp)

include_directories(${PROJECT_BINARY_DIR}/corba ${YAML-CPP_INCLUDE_DIRS})

set(sources
    IIDCCmdSVC_impl.cc
    IIDCCameraRTC.cc
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
    ${YAML-CPP_LIBRARIES}
    boost_serialization)

install(TARGETS ${PROJECT_NAME}Comp RUNTIME DESTINATION bin)

if(CNOID_FOUND)
  add_cnoid_rtc(
      ${PROJECT_NAME}RTC
      ${idl_cpp_files}
      ${sources})

  target_link_libraries(
      ${PROJECT_NAME}RTC
      ${TUIIDCPP_LIB}
      ${OPENRTM_LIBRARIES}
      ${YAML-CPP_LIBRARIES}
      boost_serialization)
endif()
