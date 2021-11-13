cmake_minimum_required(VERSION 3.10)

project(foudation)

SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -g")  # Debug模式下的编译指令
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -g")  # Release模式下的编译指令
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

link_libraries(pthread dw)
include_directories(strings/include stack_trace/include concurrency/include buffer/include utility/include)

add_subdirectory(strings)
add_subdirectory(stack_trace)
add_subdirectory(concurrency)
add_subdirectory(buffer)
add_subdirectory(utility)