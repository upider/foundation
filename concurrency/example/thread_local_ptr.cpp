#include <unistd.h>

#include <thread>
#include <iostream>

#include "concurrency/thread/thread.hpp"
#include "concurrency/thread/thread_local_ptr.hpp"

class Helper
{
public:
    int x = 100;
};

int main(int argc, char const *argv[])
{
    thread_local ThreadLocalPtr<Helper> threadLocalHelperPtr;

    std::thread th1(
        []()
        {
            auto p = threadLocalHelperPtr.get_ptr();
            std::cout << "thread id = " << current_thread_pid() << ", " << &p << std::endl;
        });

    sleep(1);
    auto p = threadLocalHelperPtr.get_ptr();
    std::cout << "thread id = " << current_thread_pid() << ", " << &p << std::endl;

    th1.join();

    return 0;
}
