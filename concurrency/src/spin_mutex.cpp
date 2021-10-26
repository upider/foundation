#include <pthread.h>

#include "mutex/spin_mutex.hpp"

SpinMutex::SpinMutex(bool process_shared)
{
    int pshared = PTHREAD_PROCESS_PRIVATE;
    if (process_shared)
    {
        pshared = PTHREAD_PROCESS_SHARED;
    }

    pthread_spin_init(&_lock, pshared);
}

SpinMutex::~SpinMutex()
{
    pthread_spin_destroy(&_lock);
}

void SpinMutex::lock()
{
    pthread_spin_lock(&_lock);
}

void SpinMutex::unlock()
{
    pthread_spin_unlock(&_lock);
}

bool SpinMutex::try_lock()
{
    return pthread_spin_trylock(&_lock) == 0 ? true : false;
}