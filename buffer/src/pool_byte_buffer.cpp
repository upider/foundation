#include "buffer/pool_byte_buffer.hpp"
#include "pool/page_heap.hpp"
#include "pool/pool_byte_buffer_allocator.hpp"
#include "pool/size_class.hpp"

PoolByteBuffer::PoolByteBuffer(std::size_t size)
    : _pbba(PoolByteBufferAllocator::instance()),
      _page_heap_wptr(PoolByteBufferAllocator::instance().get_thread_local_page_heap())
{
    auto sz = _pbba.size_class();
    auto si = sz->size_info(size);

    _page_type = si.size_type;
    _free_idx = si.free_list_index;
    _cap = si.cap;

    auto ph = _page_heap_wptr.lock();

    switch (_page_type)
    {
    case PAGE_SIZE_TYPE::SMALL:
    {
        ph->alloc_small(_free_idx, _data);
        break;
    }
    case PAGE_SIZE_TYPE::NORMAL:
    {
        ph->alloc_normal(_free_idx, _data);
        break;
    }
    case PAGE_SIZE_TYPE::BIG:
    {
        ph->alloc_huge(_free_idx, _data);
        break;
    }
    default:
    {
        ph->alloc_unmanage(_cap, _data);
        break;
    }
    }
    if (_data == nullptr)
    {
        auto ph = _page_heap_wptr.lock();
        ph->alloc_unmanage(_cap, _data);
    }
}

PoolByteBuffer::~PoolByteBuffer()
{
    auto ph = _page_heap_wptr.lock();
    if (std::this_thread::get_id() == ph->thread_id())
    {
        //"当前线程"为"内存申请线程"
        //放回free_list
        switch (_page_type)
        {
        case PAGE_SIZE_TYPE::SMALL:
        {
            ph->free_small(_free_idx, _data);
            break;
        }
        case PAGE_SIZE_TYPE::NORMAL:
        {
            ph->free_normal(_free_idx, _data);
            break;
        }
        case PAGE_SIZE_TYPE::BIG:
        {
            ph->free_huge(_free_idx, _data);
            break;
        }
        default:
        {
            ph->free_unmanage(0, _data);
            break;
        }
        }
    }
    else
    {
        if (ph == nullptr)
        {
            //"当前线程"不为"内存申请线程", "内存申请线程"不存在
            //将内存放回当前线程
            ph = _pbba.get_thread_local_page_heap();
        }
        switch (_page_type)
        {
        case PAGE_SIZE_TYPE::SMALL:
        {
            ph->thread_free_small(_free_idx, _data);
            break;
        }
        case PAGE_SIZE_TYPE::NORMAL:
        {
            ph->thread_free_normal(_free_idx, _data);
            break;
        }
        case PAGE_SIZE_TYPE::BIG:
        {
            ph->thread_free_huge(_free_idx, _data);
            break;
        }
        default:
        {
            ph->thread_free_unmanage(0, _data);
            break;
        }
        }
    }
}

std::size_t PoolByteBuffer::cap()
{
    return _cap;
}

std::size_t PoolByteBuffer::size()
{
    return _cap - _used;
}

Byte *PoolByteBuffer::data()
{
    return _data;
}