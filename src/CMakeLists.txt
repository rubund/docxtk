

add_executable(docxtk docxtk.c)

target_link_libraries(docxtk m)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

install(TARGETS docxtk DESTINATION bin)
