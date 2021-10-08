cmake_minimum_required(VERSION 3.10)

project(foudation)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -g -ggdb")  # Debug模式下的编译指令
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -Wall -g -O2")  # Release模式下的编译指令
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(strings)
add_subdirectory(byte)
add_subdirectory(concurrency)