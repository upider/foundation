#ifndef __THREAD_CACHE_HPP__
#define __THREAD_CACHE_HPP__

class Arena;

class ThreadCache
{
private:
    Arena* _arena;
    
public:
    ThreadCache(Arena *arena, int smallCacheSize, int normalCacheSize,
                int maxCachedBufferCapacity, int freeSweepAllocationThreshold);
    ~ThreadCache();

public:
    Arena *arena();
};

#endif // __THREAD_CACHE_HPP__