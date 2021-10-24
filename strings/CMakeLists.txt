aux_source_directory(src SRCS)

add_library(strings SHARED ${SRCS})

add_executable(str example/main.cpp ${SRCS})
target_include_directories(str PRIVATE include)