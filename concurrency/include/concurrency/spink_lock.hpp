#ifndef __SPIN_LOCK_HPP__
#define __SPIN_LOCK_HPP__

class SpinLock
{
private:
    volatile int _lock;

public:
    SpinLock(bool process_shared = false);
    ~SpinLock();

    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;
public:
    void lock();
    void unlock();
    bool try_lock();
};


#endif // __SPIN_LOCK_HPP__