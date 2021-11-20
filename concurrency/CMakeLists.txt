aux_source_directory(src SRCS)

add_library(concurrency SHARED ${SRCS})

add_executable(delay_queue example/delay_queue.cpp ${SRCS})

add_executable(queue example/queue.cpp ${SRCS})
target_link_libraries(queue stacktrace)

add_executable(thread_pool_executor example/thread_pool_executor.cpp ${SRCS})
target_link_libraries(thread_pool_executor stacktrace)

add_executable(priority_executor example/priority_executor.cpp ${SRCS})
target_link_libraries(priority_executor stacktrace)

add_executable(thread_local_ptr example/thread_local_ptr.cpp ${SRCS})
add_executable(thread example/thread.cpp ${SRCS})
add_executable(signal example/signal.cpp ${SRCS})
add_executable(ticker example/ticker.cpp ${SRCS})
add_executable(simple_timer example/simple_timer.cpp ${SRCS})