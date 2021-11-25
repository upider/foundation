#ifndef __SCHEDULE_EXECUTOR_HPP__
#define __SCHEDULE_EXECUTOR_HPP__

#include <chrono>

#include "concurrency/executor/executor_task.hpp"
#include "concurrency/executor/executor.hpp"
#include "concurrency/concurrent_queue/delay_queue.hpp"

template <typename Clock = std::chrono::system_clock>
class ScheduleExecutor : public Executor<DelayQueue<std::shared_ptr<ExecutorTask>, Clock>, std::shared_ptr<ExecutorTask>>
{
protected:
    virtual void run() override;

public:
    ScheduleExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory);
    virtual ~ScheduleExecutor();

    bool execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task);

    bool execute(std::chrono::time_point<Clock> &time_point, std::function<void()> &&task);

    bool try_execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task);

    bool try_execute(std::chrono::time_point<Clock> &time_point, std::function<void()> task);

    template <typename Rep, typename Period>
    bool wait_execute(std::chrono::time_point<Clock> &time_point, std::function<void()> task, const std::chrono::duration<Rep, Period> &wait_duration);

    template <typename Rep, typename Period>
    bool wait_execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration);

    /******************模板*******************/
    template <typename Rep, typename Period>
    bool execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task);

    template <typename Rep, typename Period>
    bool execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task);

    template <typename Rep, typename Period>
    bool try_execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task);

    template <typename Rep, typename Period>
    bool try_execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task);

    template <typename Rep, typename Period, typename WRep, typename WPeriod>
    bool wait_execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task, const std::chrono::duration<WRep, WPeriod> &wait_duration);

    template <typename Rep, typename Period, typename WRep, typename WPeriod>
    bool wait_execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<WRep, WPeriod> &wait_duration);
};

template <typename Clock>
void ScheduleExecutor<Clock>::run() 
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

template <typename Clock>
ScheduleExecutor<Clock>::ScheduleExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor<DelayQueue<std::shared_ptr<ExecutorTask>, Clock>, std::shared_ptr<ExecutorTask>>(threads, thread_factory) {}

template <typename Clock>
ScheduleExecutor<Clock>::~ScheduleExecutor() {}

template <typename Clock>
bool ScheduleExecutor<Clock>::execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->push(task, time_point);
        return true;
    }
}

template <typename Clock>
bool ScheduleExecutor<Clock>::execute(std::chrono::time_point<Clock> &time_point, std::function<void()> &&task)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->push(time_point, std::make_shared<FunctionExecutorTask>(std::forward<std::function<void()>>(task)));
        return true;
    }
}

template <typename Clock>
bool ScheduleExecutor<Clock>::try_execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->try_push(time_point, task);
        return true;
    }
}

template <typename Clock>
bool ScheduleExecutor<Clock>::try_execute(std::chrono::time_point<Clock> &time_point, std::function<void()> task)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->try_push(time_point, std::make_shared<FunctionExecutorTask>(task));
        return true;
    }
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::wait_execute(std::chrono::time_point<Clock> &time_point, std::function<void()> task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->wait_push(time_point, std::make_shared<FunctionExecutorTask>(task), wait_duration);
        return true;
    }
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::wait_execute(std::chrono::time_point<Clock> &time_point, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (this->_phase.load() != this->RUNNING)
    {
        return false;
    }
    else
    {
        this->_task_queue->wait_push(time_point, task, wait_duration);
        return true;
    }
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task)
{
    return execute(Clock::now() + delay, task);
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task)
{
    return execute(Clock::now() + delay, std::make_shared<FunctionExecutorTask>(task));
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::try_execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task)
{
    return try_execute(Clock::now() + delay, task);
}

template <typename Clock>
template <typename Rep, typename Period>
bool ScheduleExecutor<Clock>::try_execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task)
{
    return try_execute(Clock::now() + delay, std::make_shared<FunctionExecutorTask>(task));
}

template <typename Clock>
template <typename Rep, typename Period, typename WRep, typename WPeriod>
bool ScheduleExecutor<Clock>::wait_execute(const std::chrono::duration<Rep, Period> &delay, std::function<void()> task, const std::chrono::duration<WRep, WPeriod> &wait_duration)
{
    return try_execute(Clock::now() + delay, std::make_shared<FunctionExecutorTask>(task), wait_duration);
}

template <typename Clock>
template <typename Rep, typename Period, typename WRep, typename WPeriod>
bool ScheduleExecutor<Clock>::wait_execute(const std::chrono::duration<Rep, Period> &delay, std::shared_ptr<ExecutorTask> task, const std::chrono::duration<WRep, WPeriod> &wait_duration)
{
    return try_execute(Clock::now() + delay, task, wait_duration);
}

#endif /* __SCHEDULE_EXECUTOR_HPP__ */
