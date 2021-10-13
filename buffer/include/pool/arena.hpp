#ifndef __ARENA_HPP__
#define __ARENA_HPP__

#include <atomic>
#include <cinttypes>
#include <vector>

class SizeClasses;
class Subpage;
class PoolByteBufferAllocator;
class ChunkList;
class ByteBuffer;
class ThreadCache;

class Arena
{
public:
    enum SizeClass
    {
        Small,
        Normal
    };

private:
    PoolByteBufferAllocator *_parent;
    int _numSmallSubpagePools;
    int _alignment;
    std::vector<Subpage *> _smallSubpagePools;
    ChunkList *q050;
    ChunkList *q025;
    ChunkList *q000;
    ChunkList *qInit;
    ChunkList *q075;
    ChunkList *q100;
    // Metrics for allocations and deallocations
    std::int64_t _allocationsNormal;
    // We need to use the std::int64_t here as this is not guarded via synchronized block.
    std::int64_t _allocationsSmall;
    std::int64_t _allocationsHuge;
    std::int64_t _activeBytesHuge;
    std::int64_t _deallocationsSmall;
    std::int64_t _deallocationsNormal;

    // We need to use the std::int64_t here as this is not guarded via synchronized block.
    std::int64_t _deallocationsHuge;
    // Number of thread caches backed by this arena.
    std::atomic_int _numThreadCaches;
    SizeClasses *_sizeClasses;

public:
    /**
     * @brief Arena 构造函数
     * @param pbba PoolByteBufferAllocator
     * @param pageSize
     * @param pageShifts
     * @param chunkSize
     * @param alignment
     */
    Arena(PoolByteBufferAllocator *pbba, int pageSize, int pageShifts, int chunkSize, int alignment);
    ~Arena();

    // ByteBuffer *allocate(ThreadCache *tc, int reqCapacity, int maxCapacity);
    ByteBuffer *allocate(ThreadCache &tc, int reqCapacity, int maxCapacity);
    int numThreadCaches();
    int numSmallSubpagePools();
    SizeClasses *sizeClasses();

private:
    Subpage *newSubpagePoolHead();
    void tcacheAllocateSmall(ThreadCache &cache, ByteBuffer *buf, int reqCapacity, int sizeIdx);
    void tcacheAllocateNormal(ThreadCache &cache, ByteBuffer *buf, int reqCapacity, int sizeIdx);
    void allocateHuge(ByteBuffer *buf, int reqCapacity);
    void allocateNormal(ByteBuffer *buf, int reqCapacity, int sizeIdx, ThreadCache *threadCache);
};

#endif // __ARENA_HPP__