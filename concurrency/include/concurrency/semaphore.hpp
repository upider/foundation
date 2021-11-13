#ifndef __SEMAPHORE_HPP__
#define __SEMAPHORE_HPP__

#include <semaphore.h>

class Semaphore
{
private:
    sem_t _sem;

public:
    Semaphore(int pshared = 0, unsigned int value = 0);
    ~Semaphore();

    int post();
    int wait();
    int try_wait();

    Semaphore(const Semaphore &) = delete;
    Semaphore &operator=(const Semaphore &) = delete;
};

#endif // __SEMAPHORE_HPP__