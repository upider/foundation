#include <iostream>
#include <sys/signal.h>
#include <unistd.h>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>

#include "concurrency/mutex/null_mutex.hpp"
#include "buffer/pool_byte_buffer.hpp"
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
    std::size_t N = 1000*10;
    int blocks[] = {128, 1024, 2048, 4096};
    std::condition_variable_any cva;
    auto threads = std::vector<std::thread *>(numThreads);
    std::atomic_bool run{true};

    auto task = [&blocks, &cva, &run]()
    {
        auto t1 = std::chrono::high_resolution_clock::now();
        while(run)
        {
            NullMutex lock;
            cva.wait(lock);
            PoolByteBuffer buf2(blocks[2]);
            buf2.write<long int>(100);
            std::cout << buf2.to_str() << std::endl;
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<int64_t, std::nano> diff = t2 - t1;
        std::cout << diff.count() / 1e9 << std::endl;
    };

    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i] = new std::thread(task);
    }

    for (size_t i = 0; i < N; i++)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        cva.notify_all();
    }

    run.store(false);
    cva.notify_all();
    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i]->join();
    }

    for (size_t i = 0; i < numThreads; i++)
    {
        delete threads[i];
    }

    return 0;
}
