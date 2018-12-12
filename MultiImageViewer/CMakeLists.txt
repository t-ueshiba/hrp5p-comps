cmake_minimum_required(VERSION 3.0)
project(MultiImageViewer)

set(idls MultiImage Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba	"${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)

set(sources
    main.cc
    ImageViewerRTC.cc)

add_executable(
    ${PROJECT_NAME}Comp
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ${PROJECT_NAME}Comp
    ${TUVPP_LIB}
    ${OPENRTM_LIBRARIES})

install(TARGETS ${PROJECT_NAME}Comp RUNTIME DESTINATION bin)

