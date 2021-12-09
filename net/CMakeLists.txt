aux_source_directory(src SRCS)

# add_library(select SHARED ${SRCS})

add_executable(select example/select.cpp ${SRCS})
add_executable(acceptor example/acceptor.cpp ${SRCS})