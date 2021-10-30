#ifndef __RW_MUTEX_HPP__
#define __RW_MUTEX_HPP__

#include <pthread.h>
#include <unistd.h>

class RWMutex
{
private:
    pthread_rwlock_t _pthread_rwlock;

public:
    RWMutex();
    ~RWMutex();

    RWMutex(const RWMutex&) = delete;
    RWMutex& operator=(const RWMutex&) = delete;
public:
    int unlock();
    int read_lock();
    int write_lock();
    int try_read_lock();
    int try_write_lock();
};

class ReadGuard
{
private:
    RWMutex &_rwLock;

public:
    ReadGuard(RWMutex &rwLock);
    ~ReadGuard();
};

class WriteGuard
{
private:
    RWMutex &_rwLock;

public:
    WriteGuard(RWMutex &rwLock);
    ~WriteGuard();
};

#endif //__RW_MUTEX_HPP__