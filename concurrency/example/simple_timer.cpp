#include <iostream>

#include "concurrency/thread/thread.hpp"
#include "concurrency/timer/simple_timer.hpp"

int handler(int a, int b)
{
    std::cout << "a + b = " << a + b << std::endl;
    std::cout << current_thread_name() << std::endl;
    return 0;
}

int main(int argc, char const *argv[])
{
    SimpleTimer timer;
    // timer.schedule(std::bind(handler, 45, 54), 5, 500, false);
    timer.schedule(std::bind([](int a, int b)
                             {
                                 std::cout << "a + b = " << a + b << std::endl;
                                 std::cout << current_thread_name() << std::endl;
                             },
                             45, 54),
                   5, 500, false);

    sleep(10);
    // timer.cancel();
    return 0;
}
