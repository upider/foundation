#include <iostream>
#include <thread>

#include "concurrency/concurrent_queue/delay_queue.hpp"

int main(int argc, char const *argv[])
{
    DelayQueue<int> dlq;

    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto tp1 = now + std::chrono::seconds(1);
    auto tp2 = now + std::chrono::seconds(1);
    auto tp3 = now + std::chrono::milliseconds(100);

    std::thread producer([&]()
                         {
                             dlq.push(tp1, 6);
                             dlq.push(tp2, 4);
                             dlq.push(tp3, 100);
                         });

    std::thread consumer1([&]()
                          {
                              auto x = dlq.pop();
                              std::cout << x << std::endl;
                          });
    std::thread consumer2([&]()
                          {
                              auto x = dlq.pop();
                              std::cout << x << std::endl;
                          });
    std::thread consumer3([&]()
                          {
                              int x;
                              while(!dlq.try_pop(x)){}
                              std::cout << x << std::endl;
                          });

    std::this_thread::sleep_for(std::chrono::seconds(10));

    producer.join();
    consumer1.join();
    consumer2.join();
    consumer3.join();

    return 0;
}
