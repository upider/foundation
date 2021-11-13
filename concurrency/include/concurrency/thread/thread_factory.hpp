#ifndef __THREAD_FACTORY_HPP__
#define __THREAD_FACTORY_HPP__

#include <thread>
#include <functional>

class ThreadFactory
{
public:
    ThreadFactory() = default;
    virtual ~ThreadFactory() = default;

    virtual std::thread get(std::function<void()> &&func) = 0;
};

#endif // __THREAD_FACTORY_HPP__