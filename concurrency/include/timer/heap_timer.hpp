#ifndef __HEAP_TIMER_HPP__
#define __HEAP_TIMER_HPP__

#include "timer/timer.hpp"

//堆计时器, 内部任务队列使用堆, 默认使用单线程调度
//默认情况下不能处理时间过长的任务, 否则会阻塞后面的任务
class HeapTimer : virtual public Timer
{
private:
    std::size_t _threads;

public:
    /**
     * @param threads 线程数
     */
    HeapTimer(std::size_t threads = 1);
    /**
     * @param name 线程名, 所有线程会被命名为name-pid
     * @param threads 线程数
     */
    HeapTimer(const std::string &name, std::size_t threads = 1);
    ~HeapTimer();

public:
    //取消所有任务, 停止计时器
    virtual void cancel();
    /**
     * @brief 清空任务队列
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
    virtual void schedule(std::unique_ptr<TimerTask> task, int seconds, int micorSeconds, bool oneshot);
};

#endif // __HEAP_TIMER_HPP__