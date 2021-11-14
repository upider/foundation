#ifndef __PRIORITY_EXECUTOR_HPP__
#define __PRIORITY_EXECUTOR_HPP__

#include "concurrency/executor/priority_executor_task.hpp"
#include "concurrency/executor/executor.hpp"

template <typename T, typename Compare>
class PriorityBlockingQueue;

class TaskCompare
{
public:
    bool operator()(const std::shared_ptr<PriorityExecutorTask> t1, const std::shared_ptr<PriorityExecutorTask> t2);
};

class PriorityExecutor : public Executor<PriorityBlockingQueue<std::shared_ptr<PriorityExecutorTask>, TaskCompare>, PriorityExecutorTask>
{
protected:
    class FunctionExecutorTask : public PriorityExecutorTask
    {
    private:
        std::function<void()> _func;

    public:
        FunctionExecutorTask(std::function<void()> func, std::size_t prio);
        virtual void run();
    };

public:
    PriorityExecutor(std::size_t threads, std::unique_ptr<PriorityBlockingQueue<std::shared_ptr<PriorityExecutorTask>, TaskCompare>> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    virtual ~PriorityExecutor();

    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool execute(std::shared_ptr<PriorityExecutorTask> task);
    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool execute(std::function<void()> task, std::size_t priority);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool try_execute(std::shared_ptr<PriorityExecutorTask> task);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool try_execute(std::function<void()> task, std::size_t priority);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool wait_execute(std::function<void()> task, std::size_t priority, std::size_t timeout_sec, std::size_t timeout_nsec);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    virtual bool wait_execute(std::shared_ptr<PriorityExecutorTask> task, std::size_t timeout_sec, std::size_t timeout_nsec);
};

#endif // __PRIORITY_EXECUTOR_HPP__