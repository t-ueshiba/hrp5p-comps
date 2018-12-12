set(cnoid_uses_qt5 FALSE)
if(APPLE)
  # CNOID_INCLUDE_DIRS のいずれかの項目に "qt/5" という部分文
  # 字列が含まれているか調べることにより，choreonoid が
  # Qt4/Qt5 のどちらを使っているかを判定する．
  foreach(cnoid_include_dir ${CNOID_INCLUDE_DIRS})
    string(REGEX MATCH "qt/5" matched ${cnoid_include_dir})
    if(${matched} MATCHES "qt/5")
      set(cnoid_uses_qt5 TRUE)
    endif()
  endforeach()

  # CNOID_LDFLAGS の各項目を共有ライブラリ生成のためのリンカフ
  # ラグにコピーする．
  foreach(cnoid_ldflag ${CNOID_LDFLAGS})
    set(CMAKE_SHARED_LINKER_FLAGS
	"${CMAKE_SHARED_LINKER_FLAGS} ${cnoid_ldflag}")
  endforeach()
else()
  # CNOID_LIBRARIES に Qt5Core が含まれているか調べることにより，
  # choreonoid が Qt4/Qt5 のどちらを使っているかを判定する．
  foreach(cnoid_library ${CNOID_LIBRARIES})
    if(${cnoid_library} MATCHES Qt5Core)
      set(cnoid_uses_qt5 TRUE)
    endif()
  endforeach()
endif()
  
# choreonoid が使っている Qt のバージョンに応じたパッケージの
# 設定を読み込む．
if(cnoid_uses_qt5)
  find_package(Qt5Core)
  find_package(Qt5Widgets)
  find_package(Qt5OpenGL)
  find_package(Qt5Network)
else()
  find_package(Qt4 4.7.0 REQUIRED)
endif()

# MOC ヘッダから moc によってソースファイルを生成するマクロ.
if(cnoid_uses_qt5)
  macro(qt_wrap_cpp moc_sources)
    QT5_WRAP_CPP(moc_sources ${ARGN})
  endmacro()
else()
  macro(qt_wrap_cpp moc_sources)
    QT4_WRAP_CPP(moc_sources ${ARGN})
  endmacro()
endif()

# choreonoid の major version を調べ，plugin の場所を指定
# する sub directory 名を決定する．
string(REGEX REPLACE ".[0-9]*$" "" CNOID_MAJOR_VERSION ${CNOID_VERSION})
set(CNOID_SUBDIR choreonoid-${CNOID_MAJOR_VERSION})
set(CNOID_PLUGIN_SUBDIR lib/${CNOID_SUBDIR})

# plugin 生成コマンド
function(add_cnoid_plugin target)
  add_library(${target} SHARED ${ARGN})

  set_target_properties(${target} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR})
  install(TARGETS ${target}
    RUNTIME DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR} CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel
    LIBRARY DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR} CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel)
endfunction()

# RTC 生成コマンド
function(add_cnoid_rtc target)
  add_library(${target} SHARED ${ARGN})

  set_target_properties(${target} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CNOID_PLUGIN_SUBDIR}/rtc
    PREFIX "")
  install(TARGETS ${target}
    RUNTIME DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel
    LIBRARY DESTINATION ${CNOID_PREFIX}/${CNOID_PLUGIN_SUBDIR}/rtc CONFIGURATIONS Release Debug RelWithDebInfo MinSizeRel)
endfunction()
