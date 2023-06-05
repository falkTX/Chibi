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

find_package(PkgConfig REQUIRED)
find_package(Qt5 COMPONENTS REQUIRED Core Widgets)

if(NOT (APPLE OR WIN32))
  find_package(X11 REQUIRED)
  find_package(Qt5 COMPONENTS REQUIRED X11Extras)
else()
  set(X11_FOUND FALSE)
endif()

#######################################################################################################################
# Import carla stuff

add_subdirectory(carla/cmake)

#######################################################################################################################
# Setup Chibi target

add_executable(Chibi)

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
    $<$<BOOL:${APPLE}>:$<LINK_LIBRARY:FRAMEWORK,Cocoa.framework>>
    $<$<BOOL:${X11_FOUND}>:Qt::X11Extras>
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
)

set_target_properties(Chibi
  PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

if(APPLE)
  set_source_files_properties(
    chibiembedwidget.cpp
    PROPERTIES COMPILE_FLAGS -ObjC++)
endif()
