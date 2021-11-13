#ifndef __EXECUTOR_HPP__
#define __EXECUTOR_HPP__

#include <atomic>
#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "concurrency/thread/thread_factory.hpp"

template <typename T>
class ConcurrentQueue;
class ExecutorTask;

class Executor
{
protected:
    using ExecutorTaskSptr = std::shared_ptr<ExecutorTask>;
    class Thread
    {
    private:
        std::thread _thread;

    public:
        Thread(std::thread thread);
        ~Thread();
    };
    using ThreadUptr = std::unique_ptr<Thread>;

    std::atomic_size_t _phase{RUNNING};
    std::size_t _thread_num;
    std::unique_ptr<ConcurrentQueue<ExecutorTaskSptr>> _task_queue;
    std::string _name;
    std::shared_ptr<ThreadFactory> _thread_factory;
    std::vector<ThreadUptr> _threads;

protected:
    /**
     * @brief 初始化任务线程
     */
    virtual void init_threads();
    /**
     * @brief 任务线程运行的函数
     */
    virtual void task();

public:
    static const std::size_t RUNNING = 1UL;
    static const std::size_t SHUTDOWON = 1UL << 1;
    static const std::size_t STOP = 1UL << 2;

public:
    /**
     * @brief Construct a new Executor object
     * 
     * @param threads 最小线程数
     * @param task_queue 任务队列
     * @param thread_factory 线程工厂
     * @param name Executor name
     */
    Executor(std::size_t threads, std::unique_ptr<ConcurrentQueue<std::shared_ptr<ExecutorTask>>> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    /**
     * @brief Destroy the Executor object
     */
    virtual ~Executor();

    // /**
    //  * @brief 将定时任务任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
    //  *
    //  * @param task 要执行的任务
    //  * @param time_sec 执行时间秒
    //  * @param time_nsec 执行时间纳秒
    //  * @return true 任务放入队列成功
    //  * @return false 任务放入队列失败
    //  */
    // virtual bool schedule(std::function<void()> task, std::size_t time_sec, std::size_t time_nsec) = 0;
    // /**
    //  * @brief 将定时任务任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
    //  *
    //  * @param task 要执行的任务
    //  * @param time_sec 执行时间秒
    //  * @param time_nsec 执行时间纳秒
    //  * @return true 任务放入队列成功
    //  * @return false 任务放入队列失败
    //  */
    // virtual bool schedule(std::shared_ptr<ExecutorTask> task, std::size_t time_sec, std::size_t time_nsec) = 0;
    // /**
    //  * @brief 将定时任务放入队列, shutdown、stop或超时后, 将会失败
    //  *
    //  * @param task 要执行的任务
    //  * @param time_sec 执行时间秒
    //  * @param time_nsec 执行时间纳秒
    //  * @param timeout_sec 超时秒
    //  * @param timeout_nsec 超时纳秒
    //  * @return true 任务放入队列成功
    //  * @return false 任务放入队列失败
    //  */
    // virtual bool try_schedule(std::function<void()> task, std::size_t time_sec, std::size_t time_nsec, std::size_t timeout_sec, std::size_t timeout_nsec) = 0;
    // /**
    //  * @brief 将定时任务放入队列, shutdown、stop或超时后, 将会失败
    //  *
    //  * @param task 要执行的任务
    //  * @param time_sec 执行时间秒
    //  * @param time_nsec 执行时间纳秒
    //  * @param timeout_sec 超时秒
    //  * @param timeout_nsec 超时纳秒
    //  * @return true 任务放入队列成功
    //  * @return false 任务放入队列失败
    //  */
    // virtual bool try_schedule(std::shared_ptr<ExecutorTask> task, std::size_t time_sec, std::size_t time_nsec, std::size_t timeout_sec, std::size_t timeout_nsec) = 0;
    /**
     * @brief 不在接收新的任务, 旧的任务继续执行
     */
    void shutdown();
    /**
     * @brief 不在接收新的任务, 正在执行的任务结束后就立即停止
     */
    void stop();
    /**
     * @brief 得到线程数量
     * 
     * @return std::size_t 线程数量
     */
    std::size_t threads();
    /**
     * @brief 设置线程工厂
     * 
     * @param factory ThreadFactory
     */
    void thread_factory(std::shared_ptr<ThreadFactory> factory);
    /**
     * @brief 获取线程工厂
     * 
     * @return std::shared_ptr<ThreadFactory> ThreadFactory
     */
    std::shared_ptr<ThreadFactory> thread_factory();
};

#endif // __EXECUTOR_HPP__