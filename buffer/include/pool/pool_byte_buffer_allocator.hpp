#ifndef __POOL_BYTE_BUFFER_ALLOCTOR_HPP__
#define __POOL_BYTE_BUFFER_ALLOCTOR_HPP__

#include <limits>
#include <memory>
#include <vector>

class ByteBuffer;
class ThreadCache;
class Arena;

class PoolByteBufferAllocator
{
private:
    class ThreadLocalThreadCache
    {
    private:
        PoolByteBufferAllocator *_pbba;

    public:
        ThreadLocalThreadCache(PoolByteBufferAllocator *pbba);
        ~ThreadLocalThreadCache();

        ThreadCache &get();

    private:
        Arena *leastUsedArena(std::vector<Arena *> arenas);
    };

private:
    static const int DEFAULT_NUM_ARENA;
    static const int DEFAULT_PAGE_SIZE;
    // 8192 << 11 = 16 MiB per chunk
    static const int DEFAULT_MAX_ORDER;
    // cache sizes
    static const int DEFAULT_SMALL_CACHE_SIZE;
    static const int DEFAULT_NORMAL_CACHE_SIZE;
    // 32 kb is the default maximum capacity of the cached buffer. Similar to what is explained in
    // 'Scalable memory allocation using jemalloc'
    static const int DEFAULT_MAX_CACHED_BUFFER_CAPACITY;
    // the number of threshold of allocations when cached entries will be freed up if not frequently used
    static const int DEFAULT_CACHE_TRIM_INTERVAL;
    static const long DEFAULT_CACHE_TRIM_INTERVAL_MILLIS;
    static const bool DEFAULT_USE_CACHE_FOR_ALL_THREADS;
    static const int DEFAULT_DIRECT_MEMORY_CACHE_ALIGNMENT;
    // Use 1023 by default as we use an ArrayDeque as backing storage which will then allocate an internal array
    // of 1024 elements. Otherwise we would allocate 2048 and only use 1024 which is wasteful.
    static const int DEFAULT_MAX_CACHED_BYTEBUFFERS_PER_CHUNK;
    static const int MIN_PAGE_SIZE;
    static const int MAX_CHUNK_SIZE;

private:
    std::vector<Arena *> _arenas;
    int _smallCacheSize;
    int _normalCacheSize;
    int _chunkSize;
    ThreadLocalThreadCache *_threadLocalCache;

public:
    PoolByteBufferAllocator();
    PoolByteBufferAllocator(int nArena, int pageSize, int maxOrder, int smallCacheSize, int normalCacheSize, int directMemoryCacheAlignment);
    ~PoolByteBufferAllocator();

    /**
     * @brief allocate a ByteBuffer
     * @param initialCapacity 初始容量
     * @param maxCapacity 最大容量
     */
    ByteBuffer *buffer(int initialCapacity, int maxCapacity);

    /**
     * @brief allocate a ByteBuffer
     * @param capacity 初始容量=最大容量
     */
    ByteBuffer *buffer(int capacity);

private:
    static int validateAndCalculateChunkSize(int pageSize, int maxOrder);
    static int validateAndCalculatePageShifts(int pageSize, int alignment);
};

#endif // __POOL_BYTE_BUFFER_ALLOCTOR_HPP__