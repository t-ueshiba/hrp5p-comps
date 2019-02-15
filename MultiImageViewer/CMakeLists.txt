cmake_minimum_required(VERSION 3.0)
project(MultiImageViewer)

set(idls MultiImage Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/include/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba	"${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)

set(sources
    ImageViewerRTC.cc)

add_executable(
    ${PROJECT_NAME}Comp
    main.cc
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ${PROJECT_NAME}Comp
    ${TUVPP_LIB}
    ${OPENRTM_LIBRARIES})

install(TARGETS ${PROJECT_NAME}Comp RUNTIME DESTINATION bin)

add_library(
    ${PROJECT_NAME}RTC SHARED
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ${PROJECT_NAME}RTC
    ${TUVPP_LIB}
    ${OPENRTM_LIBRARIES})

set_target_properties(
    ${PROJECT_NAME}RTC PROPERTIES
    PREFIX "")

install(TARGETS ${PROJECT_NAME}RTC LIBRARY DESTINATION lib)
