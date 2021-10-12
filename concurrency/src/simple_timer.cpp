#include "timer/simple_timer.hpp"
#include "thread/thread.hpp"
#include "timer/time_ticker.hpp"

SimpleTimer::SimpleTimer(const std::string &name = "") : _name(name)
{
    std::function<void()> taskThread = [this]()
    {
        if (_oneshot)
        {
            _ticker->tick();
            _task->run();
        }
        else
        {
            while (!_ticker->tick())
            {
                _task->run();
            }
        }
    };
    _thread = new Thread(taskThread, name);
}

SimpleTimer::~SimpleTimer()
{
    cancel();
    if (_thread != nullptr)
    {
        delete _thread;
    }
    if (_ticker != nullptr)
    {
        delete _ticker;
    }
}

void SimpleTimer::cancel()
{
    _ticker->cancel();
}

int SimpleTimer::purge()
{
    cancel();
    _task.release();
    return 0;
}

void SimpleTimer::schedule(std::unique_ptr<TimerTask> task, int seconds, int micorSeconds, bool oneshot)
{
    _oneshot = oneshot;
    _ticker = new TimeTicker(seconds, micorSeconds);
    _task.swap(task);
    _thread->start();
}

void SimpleTimer::schedule(std::function<void()> task, int seconds, int micorSeconds, bool oneshot)
{
    _oneshot = oneshot;
    _ticker = new TimeTicker(seconds, micorSeconds);
    _task.reset(new ThreadRunnable(task));
    _thread->start();
}

SimpleTimer::ThreadRunnable::ThreadRunnable(std::function<void()> f) : _fun(std::move(f)) { }

void SimpleTimer::ThreadRunnable::run()
{
    _fun();
}