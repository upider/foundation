#ifndef __TIMER_TASK_HPP__
#define __TIMER_TASK_HPP__

class TimerTask
{
public:
    TimerTask() = default;
    virtual ~TimerTask() = default;

    virtual void run() = 0;
};

#endif // __TIMER_TASK_HPP__