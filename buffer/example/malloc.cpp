#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

#include "buffer/byte.hpp"

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
    size_t numThreads = 3;
    // auto blocks = {128};
    int blocks[] = {128, 1024, 2048, 4096};
    auto threads = std::vector<std::thread *>(numThreads);
    auto task = [&blocks]()
    {
        for (size_t i = 0; i < 1000 * 1000 * 100; i++)
        {
            Helper h1(blocks[0]);
            Helper h2(blocks[1]);
            Helper h3(blocks[2]);
            Helper h4(blocks[3]);
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
