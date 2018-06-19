cmake_minimum_required(VERSION 2.4)

set(idls MultiImage Cmd)

set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs
    ${HRPSYSBASE_PREFIX}/share/hrpsys/idl
    ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    main.cc
    ImageViewerRTC.cc)

add_executable(
    MultiImageViewerComp
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    MultiImageViewerComp
    ${TUVPP_LIB}
    ${OPENRTM_LIBRARIES})

install(TARGETS MultiImageViewerComp RUNTIME DESTINATION bin)

