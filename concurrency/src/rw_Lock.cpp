#include "lock/rw_lock.hpp"

RWLock::RWLock()
{
    pthread_rwlock_init(&_pthread_rwlock, NULL);
}

RWLock::~RWLock()
{
    pthread_rwlock_destroy(&_pthread_rwlock);
}

int RWLock::unlock()
{
    return pthread_rwlock_unlock(&_pthread_rwlock);
}

int RWLock::readLock()
{
    return pthread_rwlock_rdlock(&_pthread_rwlock);
}

int RWLock::writeLock()
{
    return pthread_rwlock_wrlock(&_pthread_rwlock);
}

int RWLock::tryReadLock()
{
    return pthread_rwlock_tryrdlock(&_pthread_rwlock);
}

int RWLock::tryWriteLock()
{
    return pthread_rwlock_trywrlock(&_pthread_rwlock);
}

ReadGuard::ReadGuard(RWLock &rwLock) : _rwLock(rwLock)
{
    _rwLock.readLock();
}

ReadGuard::~ReadGuard()
{
    _rwLock.unlock();
}

WriteGuard::WriteGuard(RWLock &rwLock) : _rwLock(rwLock)
{
    _rwLock.writeLock();
}

WriteGuard::~WriteGuard()
{
    _rwLock.unlock();
}