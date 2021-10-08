#include <iostream>
#include <stdexcept>

#include "signal_blocker.hpp"

SignalBlocker::SignalBlocker(std::initializer_list<int> sigs)
    : _sigs(sigs), _waitset(new sigset_t())
{
}

SignalBlocker::SignalBlocker(int sig)
    : _sigs({sig}), _waitset(new sigset_t())
{
}

SignalBlocker::~SignalBlocker()
{
    _thread.join();
    delete _waitset;
}

int SignalBlocker::get_signal()
{
    return _sig;
}

void SignalBlocker::beforeWait()
{
    sigemptyset(_waitset);
    for (auto i : _sigs)
    {
        sigaddset(_waitset, i);
    }
    int err = pthread_sigmask(SIG_BLOCK, _waitset, NULL);
    if (err != 0)
    {
        throw std::runtime_error("signal block error: pthread_sigmask");
    }
}

void SignalBlocker::afterWait()
{
    int err = pthread_sigmask(SIG_UNBLOCK, _waitset, NULL);
    if (err != 0)
    {
        throw std::runtime_error("signal block error: pthread_sigmask");
    }
}

void SignalBlocker::wait()
{
    beforeWait();
    int s = sigwait(_waitset, &_sig);
    if (s != 0)
    {
        throw std::runtime_error("signal block error: sigwait");
    }
    afterWait();
}

void SignalBlocker::wait(std::function<void(int)> f)
{
    beforeWait();
    int s = sigwait(_waitset, &_sig);
    f(_sig);
    if (s != 0)
    {
        throw std::runtime_error("signal block error: sigwait");
    }
    afterWait();
}

void SignalBlocker::async_wait()
{
    beforeWait();

    _thread = std::thread(
        [this]()
        {
            int s = sigwait(_waitset, &_sig);
            if (s != 0)
            {
                throw std::runtime_error("signal block error: sigwait");
            }
            afterWait();
        });
}

void SignalBlocker::async_wait(std::function<void(int)> f)
{
    beforeWait();
    auto ff = std::move(f);
    _thread = std::thread(
        [this, ff]()
        {
            int s = sigwait(_waitset, &_sig);
            try
            {
                ff(_sig);
            }
            catch (...)
            {
                throw;
            }
            if (s != 0)
            {
                throw std::runtime_error("signal block error: sigwait");
            }
            afterWait();
        });
}