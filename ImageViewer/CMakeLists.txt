cmake_minimum_required(VERSION 2.4)

set(idls Img Cmd)

set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${HRPSYSBASE_PREFIX}/share/hrpsys/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

set(sources
    main.cc
    ImageViewerRTC.cc)

add_executable(
    ImageViewerComp
    ${idl_cpp_files}
    ${sources})

target_link_libraries(
    ImageViewerComp
    ${TUVPP_LIB}
    ${OPENRTM_LIBRARIES})

install(TARGETS ImageViewerComp RUNTIME DESTINATION bin)

