#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

#include "byte/byte.hpp"

int main(int argc, char const *argv[])
{
    size_t numThreads = 10;
    // auto blocks = {128};
    auto blocks = {128, 1024, 2048};
    auto threads = std::vector<std::thread *>(numThreads);
    auto task = [&blocks]()
    {
        for (size_t i = 0; i < 1000 * 1000 * 30; i++)
        {
            for (auto s : blocks)
            {
                Byte *data = new Byte[s];
                if (data != nullptr)
                {
                    std::memset(data, 0, s);
                }
                delete[] data;
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

    for (size_t i = 0; i < numThreads; i++)
    {
        delete threads[i];
    }
    return 0;
}
