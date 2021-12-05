#include <sys/signal.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "stack_trace/stack_trace.hpp"
#include "buffer/byte.hpp"
#include "buffer/pool/size_class.hpp"
#include "buffer/pool/pool_byte_buffer_allocator.hpp"

void handler(int)
{
    std::cout << stacktrace_str(128) << std::endl;
    std::exit(-1);
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);
    
    size_t numThreads = 0;
    // auto blocks = {128};
    auto blocks = {6, 10, 11};
    auto threads = std::vector<std::thread *>(numThreads);
    auto allocator = PoolByteBufferAllocator<Byte>();

    auto task = [&allocator, &blocks]()
    {
        for (size_t i = 0; i < 1000 * 1000 * 30; i++)
        {
            for (auto idx : blocks)
            {
                Byte *data = nullptr;
                allocator.alloc_small(idx, data);
                if(data == nullptr)
                {
                    std::cout << "data == nullptr" << std::endl;
                    data = new Byte[allocator.size_class()->small_index_to_size(idx)];
                }
                allocator.free_small(idx, data);
            }
        }
    };

    auto t1 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i] = new std::thread(task);
    }

    task();

    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i]->join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<int64_t, std::nano> diff = t2 - t1;
    std::cout << "time difference： " << diff.count() << " ns"
              << " = " << diff.count() / 1e9 << " seconds" << std::endl;
    return 0;
}
