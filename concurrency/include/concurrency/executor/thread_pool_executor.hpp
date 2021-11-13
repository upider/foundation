#ifndef __THREAD_POOL_EXECUTOR_HPP__
#define __THREAD_POOL_EXECUTOR_HPP__

#include "concurrency/executor/executor_task.hpp"
#include "concurrency/executor/executor.hpp"

class ThreadPoolExecutor : public Executor
{
private:
    class FunctionExecutorTask : public ExecutorTask
    {
    private:
        std::function<void()> _func;

    public:
        FunctionExecutorTask(std::function<void()> func);
        virtual void run();
    };

public:
    ThreadPoolExecutor(std::size_t threads, std::unique_ptr<ConcurrentQueue<std::shared_ptr<ExecutorTask>>> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    virtual ~ThreadPoolExecutor();

    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool execute(std::shared_ptr<ExecutorTask> task);
    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool execute(std::function<void()> task);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool try_execute(std::shared_ptr<ExecutorTask> task);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool try_execute(std::function<void()> task);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool wait_execute(std::function<void()> task, std::size_t timeout_sec, std::size_t timeout_nsec);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool wait_execute(std::shared_ptr<ExecutorTask> task, std::size_t timeout_sec, std::size_t timeout_nsec);
};

#endif // __THREAD_POOL_EXECUTOR_HPP__