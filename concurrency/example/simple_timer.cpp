#include <iostream>

#include "thread/thread.hpp"
#include "timer/simple_timer.hpp"

int main(int argc, char const *argv[])
{
    SimpleTimer timer("test timer");
    timer.schedule(
        []()
        {
            std::cout << current_thread_name() << std::endl;
        },
        5, 500, false);

    sleep(20);
    timer.cancel();
    return 0;
}
