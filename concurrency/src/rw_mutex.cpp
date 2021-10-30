#include "mutex/rw_mutex.hpp"

RWMutex::RWMutex()
{
    pthread_rwlock_init(&_pthread_rwlock, NULL);
}

RWMutex::~RWMutex()
{
    pthread_rwlock_destroy(&_pthread_rwlock);
}

int RWMutex::unlock()
{
    return pthread_rwlock_unlock(&_pthread_rwlock);
}

int RWMutex::read_lock()
{
    return pthread_rwlock_rdlock(&_pthread_rwlock);
}

int RWMutex::write_lock()
{
    return pthread_rwlock_wrlock(&_pthread_rwlock);
}

int RWMutex::try_read_lock()
{
    return pthread_rwlock_tryrdlock(&_pthread_rwlock);
}

int RWMutex::try_write_lock()
{
    return pthread_rwlock_trywrlock(&_pthread_rwlock);
}

ReadGuard::ReadGuard(RWMutex &rwLock) : _rwLock(rwLock)
{
    _rwLock.read_lock();
}

ReadGuard::~ReadGuard()
{
    _rwLock.unlock();
}

WriteGuard::WriteGuard(RWMutex &rwLock) : _rwLock(rwLock)
{
    _rwLock.write_lock();
}

WriteGuard::~WriteGuard()
{
    _rwLock.unlock();
}