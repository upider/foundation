#ifndef __SPIN_LOCK_HPP__
#define __SPIN_LOCK_HPP__

class SpinMutex
{
private:
    volatile int _lock;

public:
    SpinMutex(bool process_shared = false);
    ~SpinMutex();

    SpinMutex(const SpinMutex&) = delete;
    SpinMutex& operator=(const SpinMutex&) = delete;
public:
    void lock();
    void unlock();
    bool try_lock();
};


#endif // __SPIN_LOCK_HPP__