#include <sys/signal.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "concurrency/thread/thread.hpp"
#include "concurrency/thread/named_thread_factory.hpp"
#include "concurrency/concurrent_queue/array_blocking_queue.hpp"
#include "concurrency/concurrent_queue/linked_blocking_queue.hpp"
#include "concurrency/executor/executor.hpp"
#include "concurrency/executor/thread_pool_executor.hpp"
#include "stack_trace/stack_trace.hpp"

void handler(int)
{
    std::cout << stacktrace_str(128) << std::endl;
    std::exit(-1);
}

class TestTask : public ExecutorTask
{
public:
    virtual void run()
    {
        std::cout << current_thread_name() << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    using queue_type = ArrayBlockingQueue<std::shared_ptr<ExecutorTask>, 10>;
    std::unique_ptr<ConcurrentQueue<std::shared_ptr<ExecutorTask>>> queue = std::unique_ptr<queue_type>(new queue_type());
    ThreadPoolExecutor *executor = new ThreadPoolExecutor(2, std::move(queue), std::make_shared<NamedThreadFactory>("test"));

    auto task = std::make_shared<TestTask>();
    executor->execute(task);

    executor->execute([]()
                      { std::cout << current_thread_name() << std::endl; });

    std::this_thread::sleep_for(std::chrono::seconds(3));
    executor->shutdown();
    delete executor;
    return 0;
}
