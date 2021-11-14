#include "concurrency/executor/priority_executor_task.hpp"

PriorityExecutorTask::PriorityExecutorTask(std::size_t prio) : _prio(prio) {}

PriorityExecutorTask::PriorityExecutorTask() {}

PriorityExecutorTask::~PriorityExecutorTask() {}

std::size_t PriorityExecutorTask::priority()
{
    return _prio;
}

void PriorityExecutorTask::priority(std::size_t prio)
{
    _prio = prio;
}
