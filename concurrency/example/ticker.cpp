#include <chrono>
#include <iostream>
#include <thread>

#include "timer/time_ticker.hpp"

int main(int argc, char const *argv[])
{
    std::chrono::time_point<std::chrono::high_resolution_clock> t1;
    std::chrono::time_point<std::chrono::high_resolution_clock> t2;
    TimeTicker tt(20, 0);
    t1 = std::chrono::high_resolution_clock::now();
    std::thread th(
        [&tt, &t1]()
        {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            tt.cancel();
        });

    tt.tick();
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<int64_t, std::nano> diff = t2 - t1;
    std::cout << "time difference： " << diff.count() << " ns"
              << " = " << diff.count() / 1e9 << "seconds" << std::endl;

    th.join();
    return 0;
}
