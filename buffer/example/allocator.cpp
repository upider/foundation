#include <iostream>
#include <thread>

#include "pool/pool_byte_buffer_allocator.hpp"

int main(int argc, char const *argv[])
{
    PoolByteBufferAllocator allocator;
    allocator.buffer(1);

    std::cout << std::thread::hardware_concurrency() << std::endl;
    return 0;
}
