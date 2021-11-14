#ifndef __EXECUTOR_HPP__
#define __EXECUTOR_HPP__

#include <atomic>
#include <memory>
#include <functional>
#include <string>
#include <vector>

#include "concurrency/thread/thread_factory.hpp"

template <typename QUEUE, typename TASK>
class Executor
{
protected:
    using TaskSptr = std::shared_ptr<TASK>;
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
    std::unique_ptr<QUEUE> _task_queue;
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
    Executor(std::size_t threads, std::unique_ptr<QUEUE> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    /**
     * @brief Destroy the Executor object
     */
    virtual ~Executor();
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

template <typename QUEUE, typename TASK>
Executor<QUEUE, TASK>::Thread::Thread(std::thread thread) : _thread(std::move(thread)) {}

template <typename QUEUE, typename TASK>
Executor<QUEUE, TASK>::Thread::~Thread()
{
    _thread.join();
}

template <typename QUEUE, typename TASK>
Executor<QUEUE, TASK>::Executor(std::size_t threads, std::unique_ptr<QUEUE> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : _thread_num(threads), _task_queue(std::move(task_queue)), _thread_factory(thread_factory), _threads(_thread_num)
{
    init_threads();
}

template <typename QUEUE, typename TASK>
Executor<QUEUE, TASK>::~Executor() {}

template <typename QUEUE, typename TASK>
void Executor<QUEUE, TASK>::init_threads()
{
    for (std::size_t i = 0; i < _thread_num; i++)
    {
        _threads.emplace_back(new Thread(_thread_factory->get(std::bind(&Executor::task, this))));
    }
}

template <typename QUEUE, typename TASK>
void Executor<QUEUE, TASK>::task()
{
    while (1)
    {
        TaskSptr task;
        switch (_phase.load())
        {
        case RUNNING:
        {
            if (_task_queue->wait_pop(task, 0, 1000))
            {
                task->run();
            }
            break;
        }
        case SHUTDOWN:
        {
            if (_task_queue->try_pop(task))
            {
                task->run();
                break;
            }
            else
            {
                return;
            }
        }
        case STOP:
        {
            return;
        }
        default:
            break;
        }
    }
}

template <typename QUEUE, typename TASK>
void Executor<QUEUE, TASK>::shutdown()
{
    _phase.store(SHUTDOWN);
}

template <typename QUEUE, typename TASK>
void Executor<QUEUE, TASK>::stop()
{
    _phase.store(STOP);
}

template <typename QUEUE, typename TASK>
std::size_t Executor<QUEUE, TASK>::threads()
{
    return _thread_num;
}

template <typename QUEUE, typename TASK>
void Executor<QUEUE, TASK>::thread_factory(std::shared_ptr<ThreadFactory> factory)
{
    _thread_factory = factory;
}

template <typename QUEUE, typename TASK>
std::shared_ptr<ThreadFactory> Executor<QUEUE, TASK>::thread_factory()
{
    return _thread_factory;
}

#endif // __EXECUTOR_HPP__