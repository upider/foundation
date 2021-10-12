#ifndef __ARENA_HPP__
#define __ARENA_HPP__

class SizeClasses;

class Arena
{
public:
    SizeClasses* sizeClasses;
public:
    Arena() {}
    ~Arena() {}
};

#endif // __ARENA_HPP__