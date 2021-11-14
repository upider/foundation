#include <sys/signal.h>
#include <iostream>
#include <chrono>
#include <thread>

#include "concurrency/thread/thread.hpp"
#include "concurrency/thread/named_thread_factory.hpp"
#include "concurrency/concurrent_queue/priority_blocking_queue.hpp"
#include "concurrency/executor/priority_executor_task.hpp"
#include "concurrency/executor/priority_executor.hpp"
#include "stack_trace/stack_trace.hpp"

void handler(int)
{
    std::cout << stacktrace_str(128) << std::endl;
    std::exit(-1);
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);

    using priority_queue_type = PriorityBlockingQueue<std::shared_ptr<PriorityExecutorTask>, TaskCompare>;
    std::unique_ptr<priority_queue_type> queue = std::unique_ptr<priority_queue_type>(new priority_queue_type());
    PriorityExecutor executor(2, std::move(queue), std::make_shared<NamedThreadFactory>("testxx"));

    executor.execute([]()
                     { std::cout << current_thread_name() << std::endl; },
                     100);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    executor.shutdown();
    return 0;
}