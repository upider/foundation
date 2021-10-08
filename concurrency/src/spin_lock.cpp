#include <pthread.h>

#include "concurrency/spink_lock.hpp"

SpinLock::SpinLock(bool process_shared)
{
    int pshared = PTHREAD_PROCESS_PRIVATE;
    if (process_shared)
    {
        pshared = PTHREAD_PROCESS_SHARED;
    }

    pthread_spin_init(&_lock, pshared);
}

SpinLock::~SpinLock()
{
    pthread_spin_destroy(&_lock);
}

void SpinLock::lock()
{
    pthread_spin_lock(&_lock);
}

void SpinLock::unlock()
{
    pthread_spin_unlock(&_lock);
}

bool SpinLock::try_lock()
{
    return pthread_spin_trylock(&_lock) == 0 ? true : false;
}