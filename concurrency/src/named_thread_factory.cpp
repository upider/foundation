#include "concurrency/thread/thread.hpp"
#include "concurrency/thread/named_thread_factory.hpp"

NamedThreadFactory::NamedThreadFactory(const std::string &name) : _name(name)
{
}

NamedThreadFactory::~NamedThreadFactory()
{
}

std::thread NamedThreadFactory::get(std::function<void()> &&func)
{
    std::string name = this->name();
    return std::thread([=](){
        current_thread_name(name);
        func();
    });
}

std::string NamedThreadFactory::name()
{
    std::size_t n = _number.load();
    _number++;
    std::string name = _name + "-" + std::to_string(n);
    return name;
}

void NamedThreadFactory::run(std::function<void()> &&func, const std::string& name) 
{
    current_thread_name(name);
    func();
}