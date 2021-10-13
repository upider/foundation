#include <limits>

#include "buffer/byte_buffer.hpp"
#include "pool/arena.hpp"
#include "pool/chunk_list.hpp"
#include "pool/size_classes.hpp"
#include "pool/subpage.hpp"
#include "pool/thread_cache.hpp"

Arena::Arena(PoolByteBufferAllocator *pbba, int pageSize, int pageShifts, int chunkSize, int alignment)
    : _parent(pbba), _alignment(alignment), _sizeClasses(new SizeClasses(pageSize, pageShifts, chunkSize, alignment))
{
    _numSmallSubpagePools = _sizeClasses->nSubpages();
    _smallSubpagePools = std::vector<Subpage *>(_numSmallSubpagePools);
    for (int i = 0; i < _numSmallSubpagePools; i++)
    {
        _smallSubpagePools[i] = newSubpagePoolHead();
    }

    q100 = new ChunkList(this, nullptr, 100, std::numeric_limits<int>::max(), chunkSize);
    q075 = new ChunkList(this, q100, 75, 100, chunkSize);
    q050 = new ChunkList(this, q075, 50, 100, chunkSize);
    q025 = new ChunkList(this, q050, 25, 75, chunkSize);
    q000 = new ChunkList(this, q025, 1, 50, chunkSize);
    qInit = new ChunkList(this, q000, std::numeric_limits<int>::min(), 25, chunkSize);

    q100->prevList(q075);
    q075->prevList(q050);
    q050->prevList(q025);
    q025->prevList(q000);
    qInit->prevList(qInit);
}

Arena::~Arena()
{
    if (_sizeClasses != nullptr)
    {
        delete _sizeClasses;
    }
}

// ByteBuffer *Arena::allocate(ThreadCache *tc, int reqCapacity, int maxCapacity)
// {
//     return nullptr;
// }

ByteBuffer *Arena::allocate(ThreadCache &tc, int reqCapacity, int maxCapacity)
{
    ByteBuffer *buf = new ByteBuffer();
    int sizeIdx = _sizeClasses->size2SizeIdx(reqCapacity);

    if (sizeIdx <= _sizeClasses->smallMaxSizeIdx())
    {
        tcacheAllocateSmall(tc, buf, reqCapacity, sizeIdx);
    }
    else if (sizeIdx < _sizeClasses->nSizes())
    {
        tcacheAllocateNormal(tc, buf, reqCapacity, sizeIdx);
    }
    else
    {
        int normCapacity = _alignment > 0 ? _sizeClasses->normalizeSize(reqCapacity) : reqCapacity;
        // Huge allocations are never served via the cache so just call allocateHuge
        allocateHuge(buf, normCapacity);
    }
    return buf;
}

int Arena::numThreadCaches()
{
    return _numThreadCaches.load();
}

int Arena::numSmallSubpagePools()
{
    return _numSmallSubpagePools;
}

Subpage *Arena::newSubpagePoolHead()
{
    Subpage *head = new Subpage();
    head->_prev = head;
    head->_next = head;
    return head;
}

void Arena::tcacheAllocateSmall(ThreadCache& cache, ByteBuffer* buf, int reqCapacity, int sizeIdx) 
{

}

void Arena::tcacheAllocateNormal(ThreadCache& cache, ByteBuffer* buf, int reqCapacity, int sizeIdx) 
{

}

void Arena::allocateHuge(ByteBuffer* buf, int reqCapacity) 
{

}

void Arena::allocateNormal(ByteBuffer* buf, int reqCapacity, int sizeIdx, ThreadCache* threadCache) 
{

}


SizeClasses* Arena::sizeClasses()
{
    return _sizeClasses;
}
