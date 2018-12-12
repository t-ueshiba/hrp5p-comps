cmake_minimum_required(VERSION 3.0)
project(TUControlPanelPlugin)

set(idls Cmd)
set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${CMAKE_SOURCE_DIR}/TU/idl)
idl_compile_cpp(idl_cpp_files idl_h_files corba	"${corba_src_dirs}" ${idls})

include_directories(${PROJECT_BINARY_DIR}/corba)

set(sources
    ButtonCmd.cc
    ControlPanelPlugin.cc
    ControlPanelRTC.cc
    ControlPanelView.cc
    Dialog.cc
    GroupBoxCmd.cc
    LabelCmd.cc
    Menu.cc
    SliderCmd.cc
    ToggleButtonCmd.cc
    createCmd.cc)

set(moc_headers
    ButtonCmd_.h
    ControlPanelView.h
    Dialog_.h
    GroupBoxCmd_.h
    LabelCmd_.h
    Menu_.h
    SliderCmd_.h
    ToggleButtonCmd_.h)

set(moc_sources "")
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
    boost_serialization
    boost_system)
