include_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${PROJECT_BINARY_DIR}/lib)

add_executable(example example.c)
target_link_libraries(example bipbuffer)
