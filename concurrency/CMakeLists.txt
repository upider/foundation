aux_source_directory(src SRCS)

add_library(concurrency SHARED ${SRCS})

add_executable(thread example/thread.cpp ${SRCS})
target_link_libraries(thread pthread)
add_executable(signal example/signal.cpp ${SRCS})
target_link_libraries(signal pthread)
add_executable(ticker example/ticker.cpp ${SRCS})
target_link_libraries(ticker pthread)
add_executable(simple_timer example/simple_timer.cpp ${SRCS})
target_link_libraries(simple_timer pthread)