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

    PriorityExecutor executor(2, std::make_shared<NamedThreadFactory>("testxx"));
    executor.start();

    executor.execute(100, []()
                     { std::cout << current_thread_name() << std::endl; });

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "shutdown" << std::endl;
    executor.stop();
    // executor.shutdown();
    return 0;
}