#include <cinttypes>
#include <limits>

#include "pool/page_heap.hpp"
#include "pool/thread_page_heap.hpp"
#include "pool/size_class.hpp"
#include "pool/pool_byte_buffer_allocator.hpp"

PoolByteBufferAllocator &PoolByteBufferAllocator::instance()
{
    static PoolByteBufferAllocator pbba;
    return pbba;
}

PoolByteBufferAllocator::PoolByteBufferAllocator() : _size_class(new SizeClass)
{
}

PoolByteBufferAllocator::~PoolByteBufferAllocator()
{
    if (_size_class != nullptr)
    {
        delete _size_class;
    }
}

void PoolByteBufferAllocator::alloc_unmanage(std::uint64_t size, Byte *&data)
{
    data = new Byte[size];
}

void PoolByteBufferAllocator::alloc_small(std::uint64_t index, Byte *&data)
{
    get_thread_local_page_heap()->alloc_small(index, data);
}

void PoolByteBufferAllocator::alloc_normal(std::uint64_t index, Byte *&data)
{
    get_thread_local_page_heap()->alloc_normal(index, data);
}

void PoolByteBufferAllocator::alloc_huge(std::uint64_t index, Byte *&data)
{
    get_thread_local_page_heap()->alloc_huge(index, data);
}

void PoolByteBufferAllocator::free_small(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->free_small(index, data);
}

void PoolByteBufferAllocator::free_normal(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->free_normal(index, data);
}

void PoolByteBufferAllocator::free_huge(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->free_huge(index, data);
}

void PoolByteBufferAllocator::free_unmanage(std::uint64_t size, Byte *data)
{
    delete[] data;
}

void PoolByteBufferAllocator::thread_free_small(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->thread_free_small(index, data);
}

void PoolByteBufferAllocator::thread_free_normal(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->thread_free_normal(index, data);
}

void PoolByteBufferAllocator::thread_free_huge(std::uint64_t index, Byte *data)
{
    get_thread_local_page_heap()->thread_free_huge(index, data);
}

void PoolByteBufferAllocator::thread_free_unmanage(std::uint64_t index, Byte *data)
{
    delete[] data;
}

SizeClass *PoolByteBufferAllocator::size_class()
{
    return _size_class;
}

std::shared_ptr<PageHeap> PoolByteBufferAllocator::get_thread_local_page_heap()
{
    thread_local ThreadPageHeap tph;
    return tph.get();
}