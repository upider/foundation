#include <unistd.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <mutex>

#include "byte/byte.hpp"
#include "concurrency/timer/time_ticker.hpp"

class Helper
{
public:
    Byte *buf;
    Helper(std::size_t size)
    {
        buf = new Byte[size];
    }
    ~Helper()
    {
        delete buf;
    }
};

int main(int argc, char const *argv[])
{
    TimeTicker ticker(0, 1000);

    size_t numThreads = 256;
    // auto blocks = {128};
    auto blocks = {128, 1024, 2048, 4096};
    auto threads = std::vector<std::thread *>(numThreads);

    std::mutex mutex;

    auto task = [&mutex, &blocks, &ticker]()
    {
        for (size_t i = 0; i < 1000; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            mutex.lock();
            // Helper h(s);
            // ticker.tick();
            mutex.unlock();
        }
    };

    auto t1 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i] = new std::thread(task);
    }

    // task();

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
