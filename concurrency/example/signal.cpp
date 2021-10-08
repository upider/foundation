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
    Signal sb({Signal::Signals::SIGNAL_TERM, Signal::Signals::SIGNAL_KILL});
    sb.wait();
    sb.wait([](int s) -> void{
        std::cout << "reveive signal: " << s << std::endl;
    });
    sb.wait(handler);

    sb.async_wait(handler);
    
    std::cout << "/* message */" << std::endl;
    while (run.load())
    {
        // std::cout << "stop running." << std::endl;
    }

    return 0;
}
