cmake_minimum_required(VERSION 2.4)

set(idls Cmd)

set(IDL_INCLUDE_DIRS ${OPENRTM_INCLUDE_DIRS})

set(idl_cpp_files "")
set(ild_h_files "")
set(corba_src_dirs ${PROJECT_SOURCE_DIR}/TU/idl)

idl_compile_cpp(idl_cpp_files idl_h_files corba
		"${corba_src_dirs}" ${idls})

include_directories(${CNOID_INCLUDE_DIRS})

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

set(headers
    ControlPanelRTC.h)

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

set(target CnoidTUControlPanelPlugin)

add_cnoid_plugin(
    ${target}
    ${idl_cpp_files}
    ${moc_sources}
    ${sources})

target_link_libraries(
    ${target}
    TUTools++
    boost_serialization
    boost_system
    ${CNOID_LIBRARIES}
    ${OPENRTM_LIBRARIES})
