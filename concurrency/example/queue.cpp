#include <sys/signal.h>
#include <unistd.h>
#include <memory>
#include <iostream>
#include <thread>

#include "stack_trace/stack_trace.hpp"
#include "concurrency/concurrent_queue/array_blocking_queue.hpp"
#include "concurrency/concurrent_queue/linked_blocking_queue.hpp"
#include "concurrency/concurrent_queue/priority_blocking_queue.hpp"

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
// ArrayBlockingQueue<int, 10> queue;
PriorityBlockingQueue<int> queue;
// LinkedBlockingQueue<int> queue(N);

void task_producer()
{
    for (size_t i = 0; i < N; i++)
    {
        queue.push(i);
        queue.wait_push(i, std::chrono::milliseconds(1));
        std::cout << std::this_thread::get_id() << " producer: " << i << std::endl;
    }
}

void task_consumer()
{
    for (size_t i = 0; i < N * 2; i++)
    {
        auto x = queue.pop();
        std::cout << std::this_thread::get_id() << " consumer: " << x << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    std::thread pro(task_producer);
    std::thread con(task_consumer);

    task_producer();

    pro.join();
    con.join();

    // sleep(5);

    return 0;
}
