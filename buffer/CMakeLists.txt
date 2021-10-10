aux_source_directory(src SRCS)
include_directories(PRIVATE include)

add_executable(byte example/byte.cpp ${SRCS})