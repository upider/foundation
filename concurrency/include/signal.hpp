#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <csignal>
#include <initializer_list>

enum class Signals : int
{
    SIGNAL_HUP = SIGHUP,
    SIGNAL_INT = SIGINT,
    SIGNAL_QUIT = SIGQUIT,
    SIGNAL_ILL = SIGILL,
    SIGNAL_SEGV = SIGSEGV,
    SIGNAL_KILL = SIGKILL,
    SIGNAL_TERM = SIGTERM,
    SIGNAL_STOP = SIGSTOP,
    SIGNAL_USR1 = SIGUSR1,
    SIGNAL_USR2 = SIGUSR2,
};

class Signal
{
private:
    sigset_t *set;

public:
    Signal(int signal)
    {
    }
    Signal(std::initializer_list<int> signals);
    Signal(int signal);
    ~Signal();

    void wait();
    void async_wait();
};

#endif // __SIGNAL_H__