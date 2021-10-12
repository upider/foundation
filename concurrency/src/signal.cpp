#include <stdexcept>

#include "signal.hpp"

void Signal::init()
{
    sigemptyset(_waitset);
    sigemptyset(_oldset);
    for (auto i : _sigs)
    {
        sigaddset(_waitset, i);
    }
}

Signal::Signal(std::initializer_list<int> sigs)
    : _sigs(sigs), _waitset(new sigset_t()), _oldset(new sigset_t())
{
    init();
}

Signal::Signal(int sig)
    : _sigs({sig}), _waitset(new sigset_t()), _oldset(new sigset_t())
{
    init();
}

Signal::Signal(const Signal &signal)
    : _sigs(signal._sigs), _waitset(new sigset_t()), _oldset(new sigset_t())
{
    init();
}

Signal::~Signal()
{
    if (_thread.joinable())
    {
        _thread.join();
    }

    delete _waitset;
    delete _oldset;
}

Signal &Signal::operator=(const Signal &signal)
{
    this->_sigs = signal._sigs;
    if (_waitset == NULL)
    {
        _waitset = new sigset_t();
    }
    if (_oldset == NULL)
    {
        _oldset = new sigset_t();
    }
    init();
    return *this;
}

void Signal::beforeWait()
{
    int err = pthread_sigmask(SIG_SETMASK, _waitset, _oldset);
    if (err != 0)
    {
        throw std::runtime_error("Signal block error: pthread_sigmask");
    }
}

void Signal::afterWait()
{
    int err = pthread_sigmask(SIG_SETMASK, _oldset, NULL);
    if (err != 0)
    {
        throw std::runtime_error("Signal block error: pthread_sigmask");
    }
}

void Signal::wait()
{
    beforeWait();
    int sig;
    int s = sigwait(_waitset, &sig);
    if (s != 0)
    {
        throw std::runtime_error("signal block error: sigwait");
    }
    afterWait();
}

void Signal::wait(std::function<void(int)> f)
{
    beforeWait();
    int sig;
    int s = sigwait(_waitset, &sig);
    f(sig);
    if (s != 0)
    {
        throw std::runtime_error("signal block error: sigwait");
    }
    afterWait();
}

void Signal::async_wait()
{
    beforeWait();

    _thread = std::thread(
        [this]()
        {
            int sig;
            int s = sigwait(_waitset, &sig);
            if (s != 0)
            {
                throw std::runtime_error("signal block error: sigwait");
            }
            afterWait();
        });
}

void Signal::async_wait(std::function<void(int)> f)
{
    beforeWait();
    auto ff = std::move(f);
    _thread = std::thread(
        [this, ff]()
        {
            int sig;
            int s = sigwait(_waitset, &sig);
            try
            {
                ff(sig);
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