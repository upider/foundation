aux_source_directory(src SRCS)

add_library(stacktrace SHARED ${SRCS})
add_executable(st example/st.cpp ${SRCS})