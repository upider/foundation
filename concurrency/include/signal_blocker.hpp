#ifndef __SIGNAL_BLOCKER_H__
#define __SIGNAL_BLOCKER_H__

#include <csignal>

#include <thread>
#include <atomic>
#include <functional>
#include <vector>

/**
 * @brief 等待信号发生
 */
class SignalBlocker
{
private:
    std::vector<int> _sigs;
    sigset_t *_waitset;
    int _sig = -1;
    std::thread _thread;

private:
    void beforeWait();
    void afterWait();

public:
    /**
     * @brief SignalBlocker 构造函数
     * @param sigs 要捕捉的信号
     */
    SignalBlocker(std::initializer_list<int> sigs);
    /**
     * @brief SignalBlocker 构造函数
     * @param sigs 要捕捉的信号
     */
    SignalBlocker(int sig);
    ~SignalBlocker();

    //获取线程收到的信号
    int get_signal();

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

#endif // __SIGNAL_BLOCKER_H__