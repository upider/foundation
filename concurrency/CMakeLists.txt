aux_source_directory(src SRCS)

add_library(concurrency SHARED ${SRCS})

add_executable(queue example/queue.cpp ${SRCS})
target_link_libraries(queue stacktrace)

add_executable(executor example/executor.cpp ${SRCS})
target_link_libraries(executor stacktrace)

add_executable(thread_local_ptr example/thread_local_ptr.cpp ${SRCS})
add_executable(thread example/thread.cpp ${SRCS})
add_executable(signal example/signal.cpp ${SRCS})
add_executable(ticker example/ticker.cpp ${SRCS})
add_executable(simple_timer example/simple_timer.cpp ${SRCS})