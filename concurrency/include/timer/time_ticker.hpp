#ifndef __TIMER_TICKER_HPP__
#define __TIMER_TICKER_HPP__

//TimeTicker 不可在多个线程调用tick()方法
class TimeTicker
{
private:
    using Timeout = struct timeval;
    using FDSet = fd_set;

private:
    Timeout *_timeout;
    int _pipefds[2];
    FDSet *_readfds;

public:
    TimeTicker(int seconds, int microSeconds = 0);
    ~TimeTicker();

public:
    /**
     * @brief tick, 等待设置的时间
     * @return bool 是否被取消
     */ 
    bool tick();
    void cancel();
};

#endif // __TIMER_TICKER_HPP__