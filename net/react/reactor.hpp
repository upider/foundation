#ifndef __REACTOR_HPP__
#define __REACTOR_HPP__

#include <thread>

class Reactor
{
private:
    std::thread _event_loop;

public:
    Reactor();
    ~Reactor();
};

#endif /* __REACTOR_HPP__ */
