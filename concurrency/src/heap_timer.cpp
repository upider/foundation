#include "timer/heap_timer.hpp"

HeapTimer::HeapTimer(std::size_t threads)
{
}

HeapTimer::HeapTimer(const std::string &name, std::size_t threads)
{
}

HeapTimer::~HeapTimer()
{
}

void HeapTimer::cancel()
{
}

int HeapTimer::purge()
{
}

void HeapTimer::schedule(std::unique_ptr<TimerTask> task, int seconds, int micorSeconds, bool oneshot) {}