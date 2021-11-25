#include "concurrency/concurrent_queue/priority_blocking_queue.hpp"
#include "concurrency/executor/executor_task.hpp"
#include "concurrency/executor/priority_executor.hpp"

void PriorityExecutor::run()
{
    while (1)
    {
        std::pair<std::shared_ptr<ExecutorTask>, int> task;
        switch (_phase.load())
        {
        case this->RUNNING:
        {
            if (_task_queue->wait_pop(task, std::chrono::milliseconds(10)))
            {
                task.first->run();
            }
            break;
        }
        case this->SHUTDOWN:
        {
            if (_task_queue->try_pop(task))
            {
                task.first->run();
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

PriorityExecutor::PriorityExecutor(std::size_t threads, std::shared_ptr<ThreadFactory> thread_factory)
    : Executor(threads, thread_factory) {}

PriorityExecutor::~PriorityExecutor() {}

bool PriorityExecutor::execute(int priority, std::shared_ptr<ExecutorTask> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        _task_queue->push(std::pair<std::shared_ptr<ExecutorTask>, int>(task, priority));
        return true;
    }
}

bool PriorityExecutor::execute(int priority, std::function<void()> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        _task_queue->push(std::make_pair<>(std::make_shared<FunctionExecutorTask>(task), priority));
        return true;
    }
}

bool PriorityExecutor::try_execute(int priority, std::shared_ptr<ExecutorTask> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->try_push(std::pair<std::shared_ptr<ExecutorTask>, int>(task, priority));
    }
}

bool PriorityExecutor::try_execute(int priority, std::function<void()> task)
{
    if (_phase != RUNNING)
    {
        return false;
    }
    else
    {
        return _task_queue->try_push(std::make_pair<>(std::make_shared<FunctionExecutorTask>(task), priority));
    }
}