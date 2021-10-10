#include <iostream>
#include <atomic>

#include "concurrency/signal.hpp"

std::atomic_bool run{true};

void handler(int s)
{
    std::cout << "reveive signal: " << s << std::endl;
    run.store(false);
}

int main(int argc, char const *argv[])
{
    //Signal::Signals::SIGNAL_INT = 2 = ctrl+c
    Signal signal({Signal::Signals::SIGNAL_INT});
    signal.wait();
    signal.wait([](int s) -> void {
        std::cout << "reveive signal: " << s << std::endl;
    });

    signal.async_wait(handler);
    
    std::cout << "/* message */" << std::endl;
    while (run.load())
    {
        // std::cout << "Running." << std::endl;
    }

    return 0;
}
