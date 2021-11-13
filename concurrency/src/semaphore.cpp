#include "concurrency/semaphore.hpp"

Semaphore::Semaphore(int pshared, unsigned int value)
{
    sem_init(&_sem, pshared, value);
}

Semaphore::~Semaphore()
{
    sem_destroy(&_sem);
}

int Semaphore::post()
{
    return sem_post(&_sem);
}

int Semaphore::wait()
{
    return sem_wait(&_sem);
}

int Semaphore::try_wait()
{
    return sem_trywait(&_sem);
}