#include "concurrency/concurrent_queue/priority_blocking_queue.hpp"
#include "concurrency/executor/priority_executor_task.hpp"
#include "concurrency/executor/priority_executor.hpp"

bool TaskCompare::operator()(const std::shared_ptr<PriorityExecutorTask> t1, const std::shared_ptr<PriorityExecutorTask> t2)
{
    return t1->priority() > t2->priority();
}

PriorityExecutor::FunctionExecutorTask::FunctionExecutorTask(std::function<void()> func, std::size_t prio)
    : PriorityExecutorTask(prio), _func(std::move(func)) {}

void PriorityExecutor::FunctionExecutorTask::run()
{
    _func();
}

PriorityExecutor::PriorityExecutor(std::size_t threads, std::unique_ptr<PriorityBlockingQueue<std::shared_ptr<PriorityExecutorTask>, TaskCompare>> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor(threads, std::move(task_queue), thread_factory) {}
    
PriorityExecutor::~PriorityExecutor() {}

bool PriorityExecutor::execute(std::shared_ptr<PriorityExecutorTask> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        _task_queue->push(task);
        return true;
    }
}

bool PriorityExecutor::execute(std::function<void()> task, std::size_t priority)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        _task_queue->push(std::make_shared<FunctionExecutorTask>(task, priority));
        return true;
    }
}

bool PriorityExecutor::try_execute(std::shared_ptr<PriorityExecutorTask> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->try_push(task);
    }
}

bool PriorityExecutor::try_execute(std::function<void()> task, std::size_t priority)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->try_push(std::make_shared<FunctionExecutorTask>(task, priority));
    }
}

bool PriorityExecutor::wait_execute(std::function<void()> task, std::size_t priority, std::size_t timeout_sec, std::size_t timeout_nsec)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->wait_push(std::make_shared<FunctionExecutorTask>(task, priority), timeout_sec, timeout_nsec);
    }
}

bool PriorityExecutor::wait_execute(std::shared_ptr<PriorityExecutorTask> task, std::size_t timeout_sec, std::size_t timeout_nsec)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->wait_push(task, timeout_sec, timeout_nsec);
    }
}