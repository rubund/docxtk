cmake_minimum_required (VERSION 2.6)
project (docxtk)

INCLUDE(CheckIncludeFiles)
find_package(PkgConfig)

pkg_check_modules(LIBZIP libzip)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-build.h)


add_subdirectory(src)
