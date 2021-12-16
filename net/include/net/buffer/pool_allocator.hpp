#ifndef __POOL_ALLOCATOR_HPP__
#define __POOL_ALLOCATOR_HPP__

#include <cinttypes>

static uint32_t roundup_pow_of_two(uint32_t size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size+1;
}

class PoolAllocator
{
private:
    /* data */
public:
    PoolAllocator(/* args */);
    ~PoolAllocator();
};

PoolAllocator::PoolAllocator(/* args */)
{
}

PoolAllocator::~PoolAllocator()
{
}


#endif /* __POOL_ALLOCATOR_HPP__ */
