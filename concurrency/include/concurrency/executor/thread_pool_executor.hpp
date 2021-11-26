#ifndef __THREAD_POOL_EXECUTOR_HPP__
#define __THREAD_POOL_EXECUTOR_HPP__

#include "concurrency/executor/executor_task.hpp"
#include "concurrency/executor/executor.hpp"

template <typename Queue>
class ThreadPoolExecutor : public Executor<Queue, std::shared_ptr<ExecutorTask>>
{
protected:
    virtual void run() override;

public:
    ThreadPoolExecutor(std::size_t threads, std::unique_ptr<Queue> task_queue, std::shared_ptr<ThreadFactory> thread_factory);
    ThreadPoolExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory);
    ~ThreadPoolExecutor();

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
    bool execute(std::function<void()> &&task);
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
    bool try_execute(std::function<void()> &&task);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @tparam Rep 时间单位的类型
     * @tparam Period 时间单位
     * @param task 要执行的任务
     * @param wait_duration 等待时间
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    template <class Rep, class Period>
    bool wait_execute(std::function<void()> &&task, const std::chrono::duration<Rep, Period> &wait_duration);
    /**
     * @brief 等待将任务放入队列, shutdown、stop或超时后, 将会失败
     * 
     * @tparam Rep 时间单位的类型
     * @tparam Period 时间单位
     * @param task 要执行的任务
     * @param wait_duration 等待时间
     * @return true 任务放入队列成功
     * @return false 任务放入队列失败
     */
    template <class Rep, class Period>
    bool wait_execute(std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration);
};

template<typename Queue>
void ThreadPoolExecutor<Queue>::run() 
{
    while (1)
    {
        std::shared_ptr<ExecutorTask> task;
        switch (this->_phase.load())
        {
        case this->RUNNING:
        {
            if (this->_task_queue->wait_pop(task, std::chrono::milliseconds(10)))
            {
                task->run();
            }
            break;
        }
        case this->SHUTDOWN:
        {
            if (this->_task_queue->try_pop(task))
            {
                task->run();
                break;
            }
            else
            {
                return;
            }
        }
        case this->STOP:
        {
            return;
        }
        default:
            break;
        }
    }
}

template <typename Queue>
ThreadPoolExecutor<Queue>::ThreadPoolExecutor(std::size_t threads, std::unique_ptr<Queue> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor<Queue, std::shared_ptr<ExecutorTask>>(threads, std::move(task_queue), thread_factory) {}

template <typename Queue>
ThreadPoolExecutor<Queue>::ThreadPoolExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor<Queue, std::shared_ptr<ExecutorTask>>(threads, thread_factory) {}

template <typename Queue>
ThreadPoolExecutor<Queue>::~ThreadPoolExecutor() {}

template <typename Queue>
bool ThreadPoolExecutor<Queue>::execute(std::shared_ptr<ExecutorTask> task)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->push(task);
        return true;
    }
}

template <typename Queue>
bool ThreadPoolExecutor<Queue>::execute(std::function<void()> &&task)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->push(std::make_shared<FunctionExecutorTask>(std::forward<std::function<void()>>(task)));
        return true;
    }
}

template <typename Queue>
bool ThreadPoolExecutor<Queue>::try_execute(std::shared_ptr<ExecutorTask> task)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        return this->_task_queue->try_push(task);
    }
}

template <typename Queue>
bool ThreadPoolExecutor<Queue>::try_execute(std::function<void()> &&task)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        return this->_task_queue->try_push(std::make_shared<FunctionExecutorTask>(std::forward<std::function<void()>>(task)));
    }
}

template <typename Queue>
template <class Rep, class Period>
bool ThreadPoolExecutor<Queue>::wait_execute(std::function<void()> &&task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        return this->_task_queue->wait_push(std::make_shared<FunctionExecutorTask>(std::forward<std::function<void()>>(task)), wait_duration);
    }
}

template <typename Queue>
template <class Rep, class Period>
bool ThreadPoolExecutor<Queue>::wait_execute(std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (this->_phase != this->RUNNING)
    {
        return false;
    }
    else
    {
        return this->_task_queue->wait_push(task, wait_duration);
    }
}

#endif /* __THREAD_POOL_EXECUTOR_HPP__ */
