#ifndef __PRIORITY_EXECUTOR_TASK_HPP__
#define __PRIORITY_EXECUTOR_TASK_HPP__

#include <memory>

#include "concurrency/executor/executor_task.hpp"

class PriorityExecutorTask : public ExecutorTask
{
protected:
    std::size_t _prio;

public:
    PriorityExecutorTask(std::size_t prio);
    PriorityExecutorTask();
    virtual ~PriorityExecutorTask();

    virtual void run() = 0;
    virtual std::size_t priority();
    virtual void priority(std::size_t prio);
};

#endif // __PRIORITY_EXECUTOR_TASK_HPP__