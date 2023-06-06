cmake_minimum_required(VERSION 3.15)
project(chibi)

set(CMAKE_POLICY_DEFAULT_CMP0025 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

#######################################################################################################################
# Find Qt

if(APPLE)
  set(MODULES_FIXME PrintSupport)
endif()

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS REQUIRED Core Widgets ${MODULES_FIXME})

if(NOT (APPLE OR WIN32))
  find_package(X11 REQUIRED)
  find_package(Qt5 COMPONENTS REQUIRED X11Extras)
else()
  set(X11_FOUND FALSE)
endif()

#######################################################################################################################
# Import carla stuff

set(CARLA_BUILD_FRAMEWORKS TRUE CACHE BOOL "Build Carla libs as frameworks")
add_subdirectory(carla/cmake)

#######################################################################################################################
# Setup Chibi target

add_executable(Chibi)

set_target_properties(Chibi
  PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(Chibi
  PRIVATE
    $<$<BOOL:${X11_FOUND}>:HAVE_X11>
    BUILDING_CARLA
    CARLA_FRONTEND_ONLY_EMBEDDABLE_PLUGINS
    #CARLA_UTILS_USE_QT
)

target_include_directories(Chibi
  PRIVATE
    .
    carla/source/backend
    carla/source/frontend
    carla/source/frontend/utils
    carla/source/includes
    carla/source/utils
)

target_link_libraries(Chibi
  PRIVATE
    carla::standalone
    carla::utils
    Qt5::Core
    Qt5::Widgets
    $<$<BOOL:${APPLE}>:$<LINK_LIBRARY:Qt5::PrintSupport>>
    $<$<BOOL:${APPLE}>:$<LINK_LIBRARY:FRAMEWORK,Cocoa.framework>>
    $<$<BOOL:${X11_FOUND}>:Qt5::X11Extras>
    $<$<BOOL:${X11_FOUND}>:X11>
)

target_sources(Chibi
  PRIVATE
    main.cpp
    chibiembedwidget.cpp
    chibiwindow.cpp
    carla/source/frontend/carla_frontend.cpp
    carla/source/frontend/pluginlist/pluginlistdialog.cpp
    carla/source/frontend/pluginlist/pluginrefreshdialog.hpp
    carla/resources/resources.qrc
    $<$<BOOL:${WIN32}>:carla/resources/ico/carla.rc>
)

if(APPLE)
  set_source_files_properties(
    chibiembedwidget.cpp
    PROPERTIES COMPILE_FLAGS -ObjC++)

  get_target_property(QtWidgetsLocation Qt5::Widgets LIB_LOCATION)
  get_filename_component(QtPluginsDir "${QtWidgetsLocation}/../../qt5/plugins" ABSOLUTE)

  add_custom_command(TARGET Chibi POST_BUILD
    COMMAND ln -sf cmake "${CMAKE_CURRENT_BINARY_DIR}/carla/lib"
  )
  add_custom_command(TARGET Chibi POST_BUILD
    COMMAND macdeployqt $<TARGET_BUNDLE_DIR:Chibi>
  )
  add_custom_command(TARGET Chibi POST_BUILD
    COMMAND mkdir -p "$<TARGET_BUNDLE_DIR:Chibi>/Contents/PlugIns/platforms" "$<TARGET_BUNDLE_DIR:Chibi>/Contents/PlugIns/styles"
  )
  add_custom_command(TARGET Chibi POST_BUILD
    COMMAND cp "${QtPluginsDir}/platforms/libqcocoa.dylib" "$<TARGET_BUNDLE_DIR:Chibi>/Contents/PlugIns/platforms/"
  )
  add_custom_command(TARGET Chibi POST_BUILD
    COMMAND cp "${QtPluginsDir}/styles/libqmacstyle.dylib" "$<TARGET_BUNDLE_DIR:Chibi>/Contents/PlugIns/styles/"
  )
endif()
