#include "concurrency/executor/executor_task.hpp"

ExecutorTask::ExecutorTask()
{
}

ExecutorTask::~ExecutorTask()
{
}

FunctionExecutorTask::FunctionExecutorTask(std::function<void()> func) : _func(std::move(func)) {}

void FunctionExecutorTask::run()
{
    _func();
}