#ifndef __RW_LOCK_HPP__
#define __RW_LOCK_HPP__

#include <pthread.h>
#include <unistd.h>

class RWLock
{
private:
    pthread_rwlock_t _pthread_rwlock;

public:
    RWLock();
    ~RWLock();

    RWLock(const RWLock&) = delete;
    RWLock& operator=(const RWLock&) = delete;
public:
    int unlock();
    int readLock();
    int writeLock();
    int tryReadLock();
    int tryWriteLock();
};

class ReadGuard
{
private:
    RWLock &_rwLock;

public:
    ReadGuard(RWLock &rwLock);
    ~ReadGuard();
};

class WriteGuard
{
private:
    RWLock &_rwLock;

public:
    WriteGuard(RWLock &rwLock);
    ~WriteGuard();
};

#endif //__RW_LOCK_HPP__