#include <iostream>

#include "concurrency/thread/thread.hpp"
#include "concurrency/thread/named_thread_factory.hpp"

int main(int argc, char const *argv[])
{
    NamedThreadFactory namedThreadFactory("test");
    auto th = namedThreadFactory.get([](){
        std::cout << "current thread name = " << current_thread_name() << std::endl;
    });
    th.join();
    return 0;
}
