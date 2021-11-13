#ifndef __NAMED_THREAD_FACTORY_HPP__
#define __NAMED_THREAD_FACTORY_HPP__

#include <string>
#include <atomic>

#include "concurrency/thread/thread_factory.hpp"

class NamedThreadFactory : public ThreadFactory
{
private:
    std::string _name;
    std::atomic<std::size_t> _number{0};

public:
    NamedThreadFactory(const std::string &name);
    ~NamedThreadFactory();

    virtual std::thread get(std::function<void()> &&func);

private:
    std::string name();
    static void run(std::function<void()> &&func, const std::string& name);
};

#endif // __NAMED_THREAD_FACTORY_HPP__