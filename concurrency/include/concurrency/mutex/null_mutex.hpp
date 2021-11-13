#ifndef __NULL_MUTEX_HPP__
#define __NULL_MUTEX_HPP__

class NullMutex
{
public:
    NullMutex();
    ~NullMutex();

    void lock();
    void unlock();
};

#endif // __NULL_MUTEX_HPP__