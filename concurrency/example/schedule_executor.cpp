#include <iostream>

#include "concurrency/thread/named_thread_factory.hpp"
#include "concurrency/executor/schedule_executor.hpp"
#include "concurrency/thread/thread.hpp"

int main(int argc, char const *argv[])
{
    ScheduleExecutor<> executor(3, std::make_shared<NamedThreadFactory>("test"));
    executor.start();

    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto time_point = now + std::chrono::seconds(2);
    executor.execute(time_point, [](){
        std::cout << current_thread_name() << std::endl;
    });

    // std::this_thread::sleep_for(std::chrono::seconds(5) + std::chrono::seconds(5));
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(5));
    executor.shutdown();
    return 0;
}
