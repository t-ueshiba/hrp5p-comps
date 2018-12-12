
function(idl_compile_cpp out_cpp_files out_header_files subdir corba_src_dirs)
  
  set(corba_dir ${PROJECT_BINARY_DIR}/${subdir})

  set(idl_names ${ARGV})
  list(REMOVE_AT idl_names 0 1 2)
    
  set(idl_flags -C ${corba_dir} -bcxx -Wbh=.hh -Wbs=Sk.cc -Wba -Wbd=DynSk.cc -nf -Wbkeep_inc_path)

  foreach(idl_include_dir ${IDL_INCLUDE_DIRS})
    set(idl_flags ${idl_flags} -I${idl_include_dir})
  endforeach()
    
  file(MAKE_DIRECTORY ${corba_dir})

  #
  # idl compile
  #
  foreach(idl_name ${idl_names})
    foreach(corba_src_dir ${corba_src_dirs})
      if(EXISTS ${corba_src_dir}/${idl_name}.idl)
        add_custom_command(
	  OUTPUT ${corba_dir}/${idl_name}.hh ${corba_dir}/${idl_name}DynSk.cc ${corba_dir}/${idl_name}Sk.cc
	  COMMAND omniidl ${idl_flags} ${corba_src_dir}/${idl_name}.idl
	  DEPENDS ${corba_src_dir}/${idl_name}.idl
	  COMMENT "Generating the C++ stubs and skeletons of ${idl_name}.idl"
	)
	set(idl_files ${idl_files} ${corba_dir}/${idl_name}.idl)
	set(idl_cpp_files
      	    ${idl_cpp_files}
	    ${corba_dir}/${idl_name}DynSk.cc
	    ${corba_dir}/${idl_name}Sk.cc)
	set(idl_h_files
	    ${idl_h_files}
	    ${corba_dir}/${idl_name}.hh)
	break()
      endif()
    endforeach()
  endforeach()
    
  set(${out_cpp_files} ${idl_cpp_files} PARENT_SCOPE)
  set(${out_header_files} ${idl_h_files} PARENT_SCOPE)

  set_source_files_properties(${idl_cpp_files} PROPERTIES GENERATED true COMPILE_FLAGS -DOMNI_UNLOADABLE_STUBS)
    
endfunction()
