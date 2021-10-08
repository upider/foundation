#include <stdexcept>
#include <chrono>

#include "thread.hpp"

Thread::ThreadRunnable::ThreadRunnable(std::function<void()> f)
    : _fun(std::move(f)) {}

void Thread::ThreadRunnable::run()
{
    _fun();
}

Thread::Thread(std::unique_ptr<Runnable> f, const std::string &name)
    : _name(name), _target(std::move(f)) {}

Thread::Thread(std::function<void()> f, const std::string &name)
    : _name(name), _target(new ThreadRunnable(std::move(f))) {}

Thread::~Thread()
{
    if (joinable())
    {
        join();
    }
}

pid_t Thread::get_pid()
{
    return _pid;
}

std::string Thread::get_name()
{
    return _name;
}

void Thread::set_name(const std::string &name)
{
    _name = name;
    tid_thread_name(_thread.get_id(), name);
}

void Thread::join()
{
    _thread.join();
}

void Thread::detach()
{
    _thread.detach();
}

bool Thread::joinable()
{
    return _thread.joinable();
}

void Thread::start()
{
    if (_running.load(std::memory_order_relaxed))
    {
        throw std::runtime_error("thrad alread running");
    }
    else
    {
        _running.store(true);
        _thread = std::thread([this]()
                              { run(); });
    }
}

void Thread::run()
{
    try
    {
        if (_target != nullptr)
        {
            init();
            _target->run();
            cleanup();
        }
    }
    catch (...)
    {
        cleanup();
        throw;
    }
}

bool Thread::is_running()
{
    return _running.load();
}

void Thread::sleep(long seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Thread::sleep(long milis, long nanos)
{
    nanos = nanos + milis * 10e6;
    std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));
}

void Thread::init()
{
    if (_name != "")
    {
        current_thread_name(_name);
    }
    else
    {
        _name = current_thread_name();
    }
    _running.store(true);
    _pid = syscall(__NR_gettid);
    _tid = _thread.get_id();
}

void Thread::cleanup()
{
    _running.store(false);
    _pid = -1;
    _target.release();
}