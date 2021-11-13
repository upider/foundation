#include "concurrency/timer/heap_timer.hpp"

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
    return 0;
}

void HeapTimer::schedule(std::function<void()> task, int seconds, int micorSeconds, bool oneshot) {}