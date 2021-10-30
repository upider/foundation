#include <sys/signal.h>
#include <memory>
#include <iostream>
#include <thread>

#include "stack_trace/stack_trace.hpp"
#include "concurrent_queue/array_blocking_queue.hpp"

void handler(int)
{
    std::cout << stacktrace_str(128) << std::endl;
    std::exit(-1);
}

class Helper
{
public:
    int x = 100;
    virtual void hello() = 0;
};

std::size_t N = 10;
ArrayBlockingQueue<int, 10> abq;

void task_producer()
{
    for (size_t i = 0; i < N; i++)
    {
        abq.push(i);
        std::cout << std::this_thread::get_id() << " producer: " << i << std::endl;
    }
}

void task_consumer()
{
    for (size_t i = 0; i < N * 2; i++)
    {
        auto x = abq.pop();
        std::cout << std::this_thread::get_id() << " consumer: " << x << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);

    std::thread pro(task_producer);
    std::thread con(task_consumer);

    task_producer();

    pro.join();
    con.join();

    return 0;
}
