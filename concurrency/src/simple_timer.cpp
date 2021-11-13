#include "concurrency/timer/simple_timer.hpp"
#include "concurrency/thread/thread.hpp"

SimpleTimer::SimpleTimer() {}

SimpleTimer::~SimpleTimer()
{
    cancel();
    _thread.join();
}

void SimpleTimer::cancel()
{
    _ticker.cancel();
}

int SimpleTimer::purge()
{
    cancel();
    return 0;
}

void SimpleTimer::schedule(std::function<void()> &&task, int seconds, int micorSeconds, bool oneshot)
{
    _oneshot = oneshot;
    _ticker = TimeTicker(seconds, micorSeconds);
    _thread = std::thread(std::bind(&SimpleTimer::run, this, std::move(task)));
}

void SimpleTimer::run(std::function<void()> task)
{
    if (_oneshot)
    {
        if (!_ticker.tick())
        {
            task();
        }
    }
    else
    {
        while (!_ticker.tick())
        {
            task();
        }
    }
}