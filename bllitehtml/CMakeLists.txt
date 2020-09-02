cmake_minimum_required(VERSION 3.8)

project(bllitehtml CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC TRUE)

find_package(Qt5Widgets REQUIRED)

set(LITEHTML_DIR "${CMAKE_CURRENT_LIST_DIR}/litehtml" CACHE PATH "Location of 'litehtml'")
set(BLEND2D_DIR "${CMAKE_CURRENT_LIST_DIR}/../../blend2d" CACHE PATH "Location of 'blend2d'")

add_definitions(-DLITEHTML_UTF8)

set(BLEND2D_STATIC TRUE)
include("${BLEND2D_DIR}/CMakeLists.txt")

add_subdirectory(thirdparty/litehtml)

add_executable(bllitehtml
  main.cpp
  blhtmlcontainer.cpp
  blhtmlcontainer.h
  blhtmlview.cpp
  blhtmlview.h)

set_property(TARGET bllitehtml PROPERTY CXX_VISIBILITY_PRESET hidden)
target_link_libraries(bllitehtml blend2d::blend2d litehtml Qt5::Widgets)
qt5_use_modules(bllitehtml Widgets)
