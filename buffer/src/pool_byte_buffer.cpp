#include "pool/pool_byte_buffer.hpp"
#include "pool/page_heap.hpp"
#include "pool/pool_byte_buffer_allocator.hpp"
#include "pool/size_class.hpp"

PoolByteBuffer::PoolByteBuffer(std::size_t size)
    : _pbba(PoolByteBufferAllocator::instance()),
      _page_heap_wptr(PoolByteBufferAllocator::instance().get_thread_local_page_heap()),
      _thread_id(std::this_thread::get_id())
{
    auto sz = _pbba.size_class();
    _page_type = sz->size_type(size);
    switch (_page_type)
    {
    case PAGE_SIZE_TYPE::SMALL:
    {
        _free_idx = sz->small_size_to_index(size);
        _cap = sz->small_index_to_size(_free_idx);
        auto ph = _page_heap_wptr.lock();
        ph->alloc_small(_free_idx, _data);
        break;
    }
    case PAGE_SIZE_TYPE::NORMAL:
    {
        _free_idx = sz->normal_size_to_index(size);
        _cap = sz->normal_index_to_size(_free_idx);
        if (auto ph = _page_heap_wptr.lock())
        {
            ph->alloc_normal(_free_idx, _data);
        }
        break;
    }
    case PAGE_SIZE_TYPE::BIG:
    {
        _free_idx = sz->huge_size_to_index(size);
        _cap = sz->huge_index_to_size(_free_idx);
        auto ph = _page_heap_wptr.lock();
        ph->alloc_huge(_free_idx, _data);
        break;
    }
    default:
    {
        _cap = size;
        auto ph = _page_heap_wptr.lock();
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
    if (_thread_id == std::this_thread::get_id())
    {
        //在本线程释放
        auto ph = _page_heap_wptr.lock();
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
        //在其他线程释放, 优先将内存放回"内存申请线程"
        //如果原线程不存在, 则放到目前运行线程
        auto ph = _page_heap_wptr.lock();
        if (ph != nullptr)
        {
            //"内存申请线程"仍存在, 放回"内存申请线程"
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
        else
        {
            //"内存申请线程"不存在, 放回"目前运行线程"
            switch (_page_type)
            {
            case PAGE_SIZE_TYPE::SMALL:
            {
                _pbba.thread_free_small(_free_idx, _data);
                break;
            }
            case PAGE_SIZE_TYPE::NORMAL:
            {
                _pbba.thread_free_normal(_free_idx, _data);
                break;
            }
            case PAGE_SIZE_TYPE::BIG:
            {
                _pbba.thread_free_huge(_free_idx, _data);
                break;
            }
            default:
            {
                _pbba.thread_free_unmanage(0, _data);
                break;
            }
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
    return _size;
}

Byte *PoolByteBuffer::data()
{
    return _data;
}