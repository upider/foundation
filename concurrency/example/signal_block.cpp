#include <iostream>
#include <atomic>

#include "signal_blocker.hpp"

std::atomic_bool run{true};

void handler(int s)
{
    std::cout << "reveive signal: " << s << std::endl;
    run.store(false);
}

int main(int argc, char const *argv[])
{
    SignalBlocker sb({SIGINT, SIGTERM});
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
