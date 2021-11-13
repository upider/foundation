#ifndef __SIMPLE_TIMER_HPP__
#define __SIMPLE_TIMER_HPP__

#include <atomic>
#include <functional>
#include <memory>
#include <future>
#include <thread>

#include "concurrency/timer/timer.hpp"
#include "concurrency/timer/time_ticker.hpp"

//简单计时器, 单进程调度任务, 只能调度一个任务
class SimpleTimer : public Timer
{
private:
    std::string _name;
    TimeTicker _ticker;
    std::thread _thread;
    bool _oneshot;

public:
    SimpleTimer();
    ~SimpleTimer();

public:
    //取消所有任务, 停止计时器
    virtual void cancel();
    /**
     * @brief 清空任务队列, 停止计时
     * @return int 清除任务数
     */
    virtual int purge();
    /**
     * @brief 添加任务
     * @param task 任务
     * @param seconds 秒
     * @param micorSeconds 微秒
     * @param oneshot 是否只调度一次
     * @return void
     */
    virtual void schedule(std::function<void()> &&task, int seconds, int micorSeconds, bool oneshot = true);

private:
    void run(std::function<void()> task);
};

#endif // __SIMPLE_TIMER_H__