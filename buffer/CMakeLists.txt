aux_source_directory(src SRCS)

add_library(buffer SHARED ${SRCS})

add_executable(byte example/byte.cpp ${SRCS})
target_link_libraries(byte buffer concurrency)

add_executable(justlock example/just_lock.cpp ${SRCS})

add_executable(pbuffer example/pbuffer.cpp ${SRCS})
target_link_libraries(pbuffer buffer concurrency stacktrace)

add_executable(pageheap example/page_heap.cpp ${SRCS})
target_link_libraries(pageheap buffer concurrency stacktrace)

add_executable(malloc example/malloc.cpp ${SRCS})
target_link_libraries(malloc buffer concurrency)

add_executable(allocator example/allocator.cpp ${SRCS})
target_link_libraries(allocator buffer concurrency stacktrace)