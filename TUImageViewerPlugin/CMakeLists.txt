cmake_minimum_required(VERSION 2.4)

if(NOT BUILD_CNOID_PLUGINS)
  return()  
endif()

set(idls Img)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${HRPSYSBASE_PREFIX}/share/hrpsys/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

include_directories(${CNOID_INCLUDE_DIRS})

set(sources
    ImageView.cc
    ImageViewerPlugin.cc
    ImageViewerRTC.cc
    setImage.cc
)

set(moc_sources "")
set(moc_headers ImageView.h)
qt_wrap_cpp(moc_sources ${moc_headers})

set(target CnoidTUImageViewerPlugin)

add_cnoid_plugin(
    ${target}
    ${idl_cpp_files}
    ${moc_sources}
    ${sources})

target_link_libraries(
    ${target}
    TUTools++
    boost_system
    ${CNOID_LIBRARIES}
    ${OPENRTM_LIBRARIES})
