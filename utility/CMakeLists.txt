aux_source_directory(src SRCS)

add_library(utility SHARED ${SRCS})

add_executable(str example/strings.cpp ${SRCS})

add_executable(defer example/defer.cpp ${SRCS})
add_executable(date example/date.cpp ${SRCS})
add_executable(env example/env.cpp ${SRCS})
add_executable(single example/single.cpp ${SRCS})