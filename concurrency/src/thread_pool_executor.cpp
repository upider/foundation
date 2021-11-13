#include "concurrency/concurrent_queue/concurrent_queue.hpp"
#include "concurrency/executor/thread_pool_executor.hpp"

ThreadPoolExecutor::FunctionExecutorTask::FunctionExecutorTask(std::function<void()> func) : _func(std::move(func)) {}

void ThreadPoolExecutor::FunctionExecutorTask::run()
{
    _func();
}

ThreadPoolExecutor::ThreadPoolExecutor(std::size_t threads, std::unique_ptr<ConcurrentQueue<std::shared_ptr<ExecutorTask>>> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor(threads, std::move(task_queue), thread_factory)
{
}

ThreadPoolExecutor::~ThreadPoolExecutor()
{
}

bool ThreadPoolExecutor::execute(std::shared_ptr<ExecutorTask> task)
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

bool ThreadPoolExecutor::execute(std::function<void()> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        _task_queue->push(std::make_shared<FunctionExecutorTask>(task));
        return true;
    }
}

bool ThreadPoolExecutor::try_execute(std::function<void()> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->try_push(std::make_shared<FunctionExecutorTask>(task));
    }
}

bool ThreadPoolExecutor::try_execute(std::shared_ptr<ExecutorTask> task)
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

bool ThreadPoolExecutor::wait_execute(std::function<void()> task, std::size_t timeout_sec, std::size_t timeout_nsec)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->wait_push(std::make_shared<FunctionExecutorTask>(task), timeout_sec, timeout_nsec);
    }
}

bool ThreadPoolExecutor::wait_execute(std::shared_ptr<ExecutorTask> task, std::size_t timeout_sec, std::size_t timeout_nsec)
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