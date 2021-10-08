aux_source_directory(src SRCS)
add_executable(strings example/main.cpp ${SRCS})
target_include_directories(strings PRIVATE include)