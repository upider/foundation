#ifndef __SIGNAL_HPP__
#define __SIGNAL_HPP__

#include <csignal>

#include <atomic>
#include <functional>
#include <thread>
#include <vector>

/**
 * @brief 等待信号发生
 */
class Signal
{
public:
    //signals
    enum Signals : int
    {
        SIGNAL_HUP = SIGHUP,
        SIGNAL_INT = SIGINT,
        SIGNAL_QUIT = SIGQUIT,
        SIGNAL_ABRT = SIGABRT,
        SIGNAL_ILL = SIGILL,
        SIGNAL_SEGV = SIGSEGV,
        SIGNAL_KILL = SIGKILL,
        SIGNAL_TERM = SIGTERM,
        SIGNAL_ALRM = SIGALRM,
        SIGNAL_STOP = SIGSTOP,
        SIGNAL_USR1 = SIGUSR1,
        SIGNAL_USR2 = SIGUSR2,
    };

private:
    std::vector<int> _sigs;
    sigset_t *_waitset;
    std::thread _thread;

private:
    void beforeWait();
    void afterWait();

public:
    /**
     * @brief Signal 构造函数
     * @param sigs 要捕捉的信号
     */
    Signal(std::initializer_list<int> sigs);
    /**
     * @brief Signal 构造函数
     * @param sigs 要捕捉的信号
     */
    Signal(int sig);
    /**
     * @brief Signal 构造函数
     * @param signal Signal
     */
    Signal(const Signal& signal);
    ~Signal();

    Signal& operator=(const Signal& signal);

    /**
     * @brief 在当前线程等待信号
     */
    void wait();
    /**
     * @brief 在当前线程等待信号
     */
    void wait(std::function<void(int)> f);
    /**
     * @brief 在新线程等待信号
     */
    void async_wait();
    /**
     * @brief 在新线程等待信号
     */
    void async_wait(std::function<void(int)> f);
};

#endif // __SIGNAL_HPP__