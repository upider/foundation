#include <iostream>
#include <sys/signal.h>
#include <unistd.h>

#include "pool/pool_byte_buffer.hpp"
#include "stack_trace/stack_trace.hpp"

void handler(int)
{
    std::cout << stacktrace_str(128) << std::endl;
    std::exit(-1);
}

int main(int argc, char const *argv[])
{
    // signal(SIGSEGV, handler);
    // signal(SIGABRT, handler);

    size_t numThreads = 3;
    // auto blocks = {128};
    int blocks[] = {128, 1024, 2048, 4096};
    auto threads = std::vector<std::thread *>(numThreads);

    auto task = [&blocks]()
    {
        for (size_t i = 0; i < 1000 * 1000 * 100; i++)
        {
            PoolByteBuffer buf1(blocks[0]);
            PoolByteBuffer buf2(blocks[1]);
            PoolByteBuffer buf3(blocks[2]);
            PoolByteBuffer buf4(blocks[3]);

            // PoolByteBuffer *buf = new PoolByteBuffer(size);
            // std::cout << "cap = " << buf->cap() << std::endl;
            // std::cout << "size = "<< buf->size() << std::endl;
            // delete buf;
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

    for (size_t i = 0; i < numThreads; i++)
    {
        delete threads[i];
    }

    return 0;
}
