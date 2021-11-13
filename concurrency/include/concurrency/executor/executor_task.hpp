#ifndef __EXECUTOR_TASK_HPP__
#define __EXECUTOR_TASK_HPP__

class ExecutorTask
{
public:
    ExecutorTask();
    ~ExecutorTask();

    virtual void run() = 0;
};

#endif // __EXECUTOR_TASK_HPP__