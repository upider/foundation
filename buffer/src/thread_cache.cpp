#include "pool/thread_cache.hpp"

ThreadCache::ThreadCache(Arena *arena, int smallCacheSize, int normalCacheSize,
                         int maxCachedBufferCapacity, int freeSweepAllocationThreshold)
{
}

ThreadCache::~ThreadCache()
{
}

Arena *ThreadCache::arena()
{
    return nullptr;
}