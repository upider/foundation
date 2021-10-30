#include <iostream>

#include "thread/thread.hpp"

thread_local int x = 100;

class MyRunnable : virtual public Runnable
{
public:
    MyRunnable() {}
    virtual void run() override
    {
        std::cout << Thread::current_thread_name() << ":" << Thread::current_thread_pid() << std::endl;
        std::cout << "&x = " << &x << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    std::cout << "&x = " << &x << std::endl;
    Thread t1([&t1]()
              { std::cout << Thread::current_thread_name() << ":" << Thread::current_thread_pid() << std::endl; },
              "t1");
    t1.start();

    Thread t2(std::shared_ptr<MyRunnable>(new MyRunnable()), "t2");
    t2.start();

    std::cout << Thread::current_thread_name() << ":" << Thread::current_thread_pid() << std::endl;
    return 0;
}
