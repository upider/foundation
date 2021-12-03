#ifndef __EXECUTOR_TASK_HPP__
#define __EXECUTOR_TASK_HPP__

#include <functional>
#include <memory>

class ExecutorTask
{
public:
    ExecutorTask();
    ~ExecutorTask();

    //TODO: run改为operator()
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

template<typename Comparator>
class PairExecutorTaskComparator
{
public:
    template<typename T>
    bool operator()(const std::pair<std::shared_ptr<ExecutorTask>, T>& x, const std::pair<std::shared_ptr<ExecutorTask>, T>& y);
};

template<typename Comparator>
template<typename T>
bool PairExecutorTaskComparator<Comparator>::operator()(const std::pair<std::shared_ptr<ExecutorTask>, T>& x, const std::pair<std::shared_ptr<ExecutorTask>, T>& y) 
{
    return Comparator()(x.second, y.second);
}

#endif /* __EXECUTOR_TASK_HPP__ */
