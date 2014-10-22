cmake_minimum_required (VERSION 2.6)
project (graywolf)

INCLUDE(CheckIncludeFiles)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-build.h)


add_subdirectory(src)
