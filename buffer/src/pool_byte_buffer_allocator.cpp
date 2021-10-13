#include "pool/pool_byte_buffer_allocator.hpp"
#include "pool/arena.hpp"
#include "pool/thread_cache.hpp"

#include <cmath>
#include <stdexcept>
#include <thread>

const int PoolByteBufferAllocator::DEFAULT_NUM_ARENA = std::thread::hardware_concurrency() * 2;
const int PoolByteBufferAllocator::DEFAULT_PAGE_SIZE = 8192;
const int PoolByteBufferAllocator::DEFAULT_MAX_ORDER = 11; // 8192 << 11 = 16 MiB per chunk
const int PoolByteBufferAllocator::DEFAULT_SMALL_CACHE_SIZE = 256;
const int PoolByteBufferAllocator::DEFAULT_NORMAL_CACHE_SIZE = 64;
const int PoolByteBufferAllocator::DEFAULT_MAX_CACHED_BUFFER_CAPACITY = 32 * 1024;
const int PoolByteBufferAllocator::DEFAULT_CACHE_TRIM_INTERVAL = 8192;
const std::int64_t PoolByteBufferAllocator::DEFAULT_CACHE_TRIM_INTERVAL_MILLIS = 0;
const bool PoolByteBufferAllocator::DEFAULT_USE_CACHE_FOR_ALL_THREADS = true;
const int PoolByteBufferAllocator::DEFAULT_DIRECT_MEMORY_CACHE_ALIGNMENT = 0;
const int PoolByteBufferAllocator::DEFAULT_MAX_CACHED_BYTEBUFFERS_PER_CHUNK = 1023;
const int PoolByteBufferAllocator::MIN_PAGE_SIZE = 4096;
const int PoolByteBufferAllocator::MAX_CHUNK_SIZE = (int)(((std::int64_t)std::numeric_limits<int>::max() + 1) / 2);

PoolByteBufferAllocator::PoolByteBufferAllocator()
{
}

PoolByteBufferAllocator::PoolByteBufferAllocator(int nArena, int pageSize, int maxOrder, int smallCacheSize, int normalCacheSize, int directMemoryCacheAlignment)
    : _arenas(nArena), _smallCacheSize(smallCacheSize), _normalCacheSize(normalCacheSize), _threadLocalCache(new ThreadLocalThreadCache(this))
{
    _chunkSize = validateAndCalculateChunkSize(pageSize, maxOrder);
    int pageShifts = validateAndCalculatePageShifts(pageSize, directMemoryCacheAlignment);

    for (int i = 0; i < nArena; i++)
    {
        auto arena = new Arena(this, pageSize, pageShifts, _chunkSize, directMemoryCacheAlignment);
        _arenas[i] = arena;
    }
}

PoolByteBufferAllocator::~PoolByteBufferAllocator()
{
    for (size_t i = 0; i < _arenas.size(); i++)
    {
        if (_arenas[i] != nullptr)
        {
            delete _arenas[i];
        }
    }
    if (_threadLocalCache != nullptr)
    {
        delete _threadLocalCache;
    }
}

int PoolByteBufferAllocator::validateAndCalculatePageShifts(int pageSize, int alignment)
{
    if (pageSize < MIN_PAGE_SIZE)
    {
        throw std::runtime_error("pageSize: " + std::to_string(pageSize) + " (expected: " + std::to_string(MIN_PAGE_SIZE) + ')');
    }

    if ((pageSize & pageSize - 1) != 0)
    {
        throw std::runtime_error("pageSize: " + std::to_string(pageSize) + " (expected: power of 2)");
    }

    if (pageSize < alignment)
    {
        throw std::runtime_error("Alignment cannot be greater than page size. Alignment: " + std::to_string(alignment) + ", page size: " + std::to_string(pageSize) + '.');
    }

    // Logarithm base 2. At this point we know that pageSize is a power of two.
    // return std::numeric_limits::max().SIZE - 1 - Integer.numberOfLeadingZeros(pageSize);
    return floor(log2(pageSize));
}

int PoolByteBufferAllocator::validateAndCalculateChunkSize(int pageSize, int maxOrder)
{
    if (maxOrder > 14)
    {
        throw std::runtime_error("maxOrder: " + std::to_string(maxOrder) + " (expected: 0-14)");
    }

    // Ensure the resulting chunkSize does not overflow.
    int chunkSize = pageSize;
    for (int i = maxOrder; i > 0; i--)
    {
        if (chunkSize > MAX_CHUNK_SIZE / 2)
        {
            char str[256];
            sprintf(str, "pageSize (%d) << maxOrder (%d) must not exceed %d", pageSize, maxOrder, MAX_CHUNK_SIZE);
            throw std::runtime_error(str);
        }
        chunkSize <<= 1;
    }
    return chunkSize;
}

ByteBuffer *PoolByteBufferAllocator::buffer(int initialCapacity, int maxCapacity)
{
    auto threadCache = _threadLocalCache->get();
    Arena *arena = _threadLocalCache->get().arena();
    auto buf = arena->allocate(threadCache, initialCapacity, maxCapacity);
    return buf;
}

ByteBuffer *PoolByteBufferAllocator::buffer(int capacity)
{
    auto threadCache = _threadLocalCache->get();
    Arena *arena = _threadLocalCache->get().arena();
    auto buf = arena->allocate(threadCache, capacity, capacity);
    return buf;
}

PoolByteBufferAllocator::ThreadLocalThreadCache::ThreadLocalThreadCache(PoolByteBufferAllocator *pbba) : _pbba(pbba)
{
}

PoolByteBufferAllocator::ThreadLocalThreadCache::~ThreadLocalThreadCache()
{
}

ThreadCache &PoolByteBufferAllocator::ThreadLocalThreadCache::get()
{
    static thread_local ThreadCache tc(leastUsedArena(_pbba->_arenas), _pbba->_smallCacheSize, _pbba->_normalCacheSize, DEFAULT_MAX_CACHED_BUFFER_CAPACITY, DEFAULT_CACHE_TRIM_INTERVAL);
    return tc;
}

Arena *PoolByteBufferAllocator::ThreadLocalThreadCache::leastUsedArena(std::vector<Arena *> arenas)
{
    Arena *minArena = arenas[0];
    for (int i = 1; i < arenas.size(); i++)
    {
        Arena *arena = arenas[i];
        if (arena->numThreadCaches() < minArena->numThreadCaches())
        {
            minArena = arena;
        }
    }

    return minArena;
}
