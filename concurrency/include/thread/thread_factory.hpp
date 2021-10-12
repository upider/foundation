#ifndef __THREAD_FACTORY_HPP__
#define __THREAD_FACTORY_HPP__

#include <functional>

using Func = std::function<void()>;

class Thread;

class ThreadFactory
{
public:
    ThreadFactory() = default;
    virtual ~ThreadFactory()  = default;

    virtual Thread newThread(Func&& func) = 0;
};

#endif // __THREAD_FACTORY_HPP__