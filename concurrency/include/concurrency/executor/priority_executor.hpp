#ifndef __PRIORITY_EXECUTOR_HPP__
#define __PRIORITY_EXECUTOR_HPP__

#include "concurrency/executor/executor.hpp"

template <typename T, typename Compare>
class PriorityBlockingQueue;
template<typename T>
class PairExecutorTaskComparator;
class ExecutorTask;

class PriorityExecutor : public Executor<PriorityBlockingQueue<std::pair<std::shared_ptr<ExecutorTask>, int>, PairExecutorTaskComparator<std::greater<int>>>, std::pair<std::shared_ptr<ExecutorTask>, int>>
{
protected:
    void run() override;

public:
    PriorityExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory);
    ~PriorityExecutor();

    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool execute(int priority, std::shared_ptr<ExecutorTask> task);
    /**
     * @brief 将任务放入队列, shutdown或stop后, 将会失败, 否则方法将会一致阻塞到task放进任务队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool execute(int priority, std::function<void()> task);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool try_execute(int priority, std::shared_ptr<ExecutorTask> task);
    /**
     * @brief 尝试将任务放入队列
     * 
     * @param task 要执行的任务
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    bool try_execute(int priority, std::function<void()> task);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    template <class Rep, class Period>
    bool wait_execute(int priority, std::function<void()> task, const std::chrono::duration<Rep, Period> &wait_duration);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @param task 要执行的任务
     * @param timeout_sec 入队超时秒
     * @param timeout_nsec 入队超时纳秒
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    template <class Rep, class Period>
    bool wait_execute(int priority, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration);
};

template <class Rep, class Period>
bool PriorityExecutor::wait_execute(int priority, std::function<void()> task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->wait_push(std::make_pair<>(std::make_shared<FunctionExecutorTask>(task), priority), wait_duration);
    }
}

template <class Rep, class Period>
bool PriorityExecutor::wait_execute(int priority, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->wait_push(std::pair<std::shared_ptr<ExecutorTask>, int>(task, priority), wait_duration);
    }
}

#endif /* __PRIORITY_EXECUTOR_HPP__ */
