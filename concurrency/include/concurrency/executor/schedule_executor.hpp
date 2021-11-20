#ifndef __SCHEDULE_EXECUTOR_HPP__
#define __SCHEDULE_EXECUTOR_HPP__

#include "concurrency/executor/executor.hpp"

class ScheduleExecutor : public Executor
{
private:
    
    void task();

public:
    ScheduleExecutor();
    ~ScheduleExecutor();
};

void ScheduleExecutor::task() 
{
    
}

ScheduleExecutor::ScheduleExecutor()
{
}

ScheduleExecutor::~ScheduleExecutor()
{
}

#endif /* __SCHEDULE_EXECUTOR_HPP__ */
