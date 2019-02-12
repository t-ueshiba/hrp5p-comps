cmake_minimum_required(VERSION 3.0)
project(TUMultiImageViewerPlugin)

set(idls MultiImage)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/include/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba	"${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)
link_directories(${CNOID_LIBRARY_DIRS})

set(sources
    ImageView.cc
    MultiImageViewerPlugin.cc
    ImageViewerRTC.cc
    setImage.cc
)

set(moc_sources "")
set(moc_headers ImageView.h)
qt_wrap_cpp(moc_sources ${moc_headers})

set(target Cnoid${PROJECT_NAME})

add_cnoid_plugin(
    ${target}
    ${idl_cpp_files}
    ${moc_sources}
    ${sources})

target_link_libraries(
    ${target}
    ${TUTOOLSPP_LIB}
    ${CNOID_LIBRARIES}
    ${OPENRTM_LIBRARIES}
    boost_system)
