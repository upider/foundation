#ifndef __EXECUTOR_TASK_HPP__
#define __EXECUTOR_TASK_HPP__

#include <functional>

class ExecutorTask
{
public:
    ExecutorTask();
    ~ExecutorTask();

    virtual void run() = 0;
};

class FunctionExecutorTask : public ExecutorTask
{
private:
    std::function<void()> _func;

public:
    FunctionExecutorTask(std::function<void()> func);
    virtual void run();
};

#endif // __EXECUTOR_TASK_HPP__