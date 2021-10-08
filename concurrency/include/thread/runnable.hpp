#ifndef __RUNNABLE_HPP__
#define __RUNNABLE_HPP__

class Runnable
{
public:
    Runnable() = default;
    virtual ~Runnable() = default;

    virtual void run() = 0;
};

#endif // __RUNNABLE_HPP__