#ifndef __EXECUTOR_HPP__
#define __EXECUTOR_HPP__

#include <atomic>
#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "concurrency/thread/thread_factory.hpp"

template <typename Queue, typename Task>
class Executor
{
protected:
    class Thread
    {
    private:
        std::thread _thread;

    public:
        Thread(std::thread thread);
        ~Thread();
    };
    using ThreadUptr = std::unique_ptr<Thread>;

    std::atomic_size_t _phase{STOP};
    std::size_t _thread_num;
    std::unique_ptr<Queue> _task_queue;
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
    virtual void run() = 0;

public:
    static const std::size_t RUNNING = 1UL;
    static const std::size_t SHUTDOWN = 1UL << 1;
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
    Executor(std::size_t threads, std::unique_ptr<Queue> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    /**
     * @brief Construct a new Executor object
     * 
     * @param threads 最小线程数
     * @param thread_factory 线程工厂
     */
    Executor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory);
    /**
     * @brief Destroy the Executor object
     */
    virtual ~Executor();
    virtual void start();
    /**
     * @brief 不在接收新的任务, 旧的任务继续执行
     */
    virtual void shutdown();
    /**
     * @brief 不在接收新的任务, 正在执行的任务结束后就立即停止
     */
    virtual void stop();
    /**
     * @brief 得到线程数量
     * 
     * @return std::size_t 线程数量
     */
    virtual std::size_t threads();
    /**
     * @brief 设置线程工厂
     * 
     * @param factory ThreadFactory
     */
    virtual void thread_factory(std::shared_ptr<ThreadFactory> factory);
    /**
     * @brief 获取线程工厂
     * 
     * @return std::shared_ptr<ThreadFactory> ThreadFactory
     */
    virtual std::shared_ptr<ThreadFactory> thread_factory();
};

template <typename Queue, typename Task>
Executor<Queue, Task>::Thread::Thread(std::thread thread) : _thread(std::move(thread)) {}

template <typename Queue, typename Task>
Executor<Queue, Task>::Thread::~Thread()
{
    _thread.join();
}

template <typename Queue, typename Task>
Executor<Queue, Task>::Executor(std::size_t threads, std::unique_ptr<Queue> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : _thread_num(threads), _task_queue(std::move(task_queue)), _thread_factory(thread_factory), _threads(_thread_num) {}

template <typename Queue, typename Task>
Executor<Queue, Task>::Executor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory)
    : _thread_num(threads), _task_queue(new Queue()), _thread_factory(thread_factory), _threads(_thread_num) {}

template <typename Queue, typename Task>
Executor<Queue, Task>::~Executor() {}

template <typename Queue, typename Task>
void Executor<Queue, Task>::start()
{
    _phase.store(RUNNING);
    init_threads();
}

template <typename Queue, typename Task>
void Executor<Queue, Task>::init_threads()
{
    for (std::size_t i = 0; i < _thread_num; i++)
    {
        _threads.emplace_back(new Thread(_thread_factory->get(std::bind(&Executor::run, this))));
    }
}

template <typename Queue, typename Task>
void Executor<Queue, Task>::shutdown()
{
    _phase.store(SHUTDOWN);
}

template <typename Queue, typename Task>
void Executor<Queue, Task>::stop()
{
    _phase.store(STOP);
}

template <typename Queue, typename Task>
std::size_t Executor<Queue, Task>::threads()
{
    return _thread_num;
}

template <typename Queue, typename Task>
void Executor<Queue, Task>::thread_factory(std::shared_ptr<ThreadFactory> factory)
{
    _thread_factory = factory;
}

template <typename Queue, typename Task>
std::shared_ptr<ThreadFactory> Executor<Queue, Task>::thread_factory()
{
    return _thread_factory;
}

#endif /* __EXECUTOR_HPP__ */
