

LINK_DIRECTORIES(${LIBZIP_LIBRARY_DIRS})

add_executable(docxtk docxtk.c)


target_link_libraries(docxtk ${LIBZIP_LIBRARIES})

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${LIBZIP_INCLUDE_DIRS})

install(TARGETS docxtk DESTINATION bin)
