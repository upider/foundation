#ifndef __TIMER_HPP__
#define __TIMER_HPP__

#include <memory>
#include <string>
#include <functional>

class TimerTask;

class Timer
{
public:
    Timer();
    /**
     * @param threads 线程数
     */
    Timer(std::size_t threads);
    /**
     * @param name 线程名, 所有线程会被命名为name-pid
     * @param threads 线程数
     */
    Timer(const std::string &name, std::size_t threads);
    virtual ~Timer();

public:
    //取消所有任务, 停止计时器
    virtual void cancel() = 0;
    /**
     * @brief 清空任务队列
     * @return int 清除任务数
     */
    virtual int purge() = 0;
    /**
     * @brief 添加任务
     * @param task 任务
     * @param seconds 秒
     * @param micorSeconds 微秒
     * @param oneshot 是否只调度一次
     * @return void
     */
    virtual void schedule(std::unique_ptr<TimerTask> task, int seconds, int micorSeconds, bool oneshot) = 0;
            /**
     * @brief 添加任务
     * @param task 任务
     * @param seconds 秒
     * @param micorSeconds 微秒
     * @param oneshot 是否只调度一次
     * @return void
     */
    virtual void schedule(std::function<void()> task, int seconds, int micorSeconds, bool oneshot = true) = 0;
};

#endif // __TIMER_HPP__