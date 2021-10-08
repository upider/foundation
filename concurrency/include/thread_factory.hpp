#ifndef __THREAD_FACTORY_H__
#define __THREAD_FACTORY_H__

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

#endif // __THREAD_FACTORY_H__