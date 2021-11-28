#ifndef __POOL_BYTE_BUFFER_ALLOCATOR_HPP__
#define __POOL_BYTE_BUFFER_ALLOCATOR_HPP__

#include <memory>

#include "pool/thread_page_heap.hpp"
#include "pool/page_heap.hpp"
#include "pool/size_class.hpp"

template <typename T>
class PoolByteBufferAllocator
{
private:
    static SizeClass *_size_class;
    std::weak_ptr<PageHeap<T>> _page_heap_wptr = get_thread_local_page_heap();

public:
    typedef T value_type;
    typedef T *pointer;

    PoolByteBufferAllocator();
    PoolByteBufferAllocator(const PoolByteBufferAllocator &);
    PoolByteBufferAllocator &operator=(const PoolByteBufferAllocator &);
    ~PoolByteBufferAllocator();

    T *allocate(std::size_t n);
    void deallocate(T *p, std::size_t n);

    /**
     * @brief 申请一块新的size大小的内存, 在缓存为空时使用
     * @param size 内存大小
     * @param data 申请内存指针
     */
    void alloc_unmanage(std::uint64_t size, T *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_small(std::uint64_t index, T *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_normal(std::uint64_t index, T *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_huge(std::uint64_t index, T *&data);

    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_small(std::uint64_t index, T *data);
    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_normal(std::uint64_t index, T *data);
    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_huge(std::uint64_t index, T *data);
    /**
     * @brief 本线程返还内存
     * @param size 内存size
     * @param data 申请内存指针
     */
    void free_unmanage(std::uint64_t size, T *data);

    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_small(std::uint64_t index, T *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_normal(std::uint64_t index, T *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_huge(std::uint64_t index, T *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_unmanage(std::uint64_t index, T *data);

    SizeClass *size_class();

    std::shared_ptr<PageHeap<T>> get_thread_local_page_heap();
};

template <typename T>
SizeClass *PoolByteBufferAllocator<T>::_size_class = new SizeClass();

template <typename T>
PoolByteBufferAllocator<T>::PoolByteBufferAllocator() {}

template <typename T>
PoolByteBufferAllocator<T>::PoolByteBufferAllocator(const PoolByteBufferAllocator &ele) {}

template <typename T>
PoolByteBufferAllocator<T> &PoolByteBufferAllocator<T>::operator=(const PoolByteBufferAllocator<T> &ele) { return *this; }

template <typename T>
PoolByteBufferAllocator<T>::~PoolByteBufferAllocator() {}

template <typename T>
T *PoolByteBufferAllocator<T>::allocate(std::size_t n)
{
    auto si = _size_class->size_info(n);

    auto page_type = si.size_type;
    auto free_idx = si.free_list_index;
    auto cap = si.cap;

    auto ph = _page_heap_wptr.lock();
    T *bytes_ptr = nullptr;

    switch (page_type)
    {
    case PAGE_SIZE_TYPE::SMALL:
    {
        ph->alloc_small(free_idx, bytes_ptr);
        break;
    }
    case PAGE_SIZE_TYPE::NORMAL:
    {
        ph->alloc_normal(free_idx, bytes_ptr);
        break;
    }
    case PAGE_SIZE_TYPE::BIG:
    {
        ph->alloc_huge(free_idx, bytes_ptr);
        break;
    }
    default:
    {
        ph->alloc_unmanage(cap, bytes_ptr);
        break;
    }
    }
    if (bytes_ptr == nullptr)
    {
        auto ph = _page_heap_wptr.lock();
        ph->alloc_unmanage(cap, bytes_ptr);
    }

    return bytes_ptr;
}

template <typename T>
void PoolByteBufferAllocator<T>::deallocate(T *p, std::size_t n)
{
    auto si = _size_class->size_info(n);
    auto page_type = si.size_type;
    auto free_idx = si.free_list_index;
    auto ph = _page_heap_wptr.lock();

    if (std::this_thread::get_id() == ph->thread_id())
    {
        //"当前线程"为"内存申请线程"
        //放回free_list
        switch (page_type)
        {
        case PAGE_SIZE_TYPE::SMALL:
        {
            ph->free_small(free_idx, p);
            break;
        }
        case PAGE_SIZE_TYPE::NORMAL:
        {
            ph->free_normal(free_idx, p);
            break;
        }
        case PAGE_SIZE_TYPE::BIG:
        {
            ph->free_huge(free_idx, p);
            break;
        }
        default:
        {
            ph->free_unmanage(0, p);
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
            ph = get_thread_local_page_heap();
        }
        switch (page_type)
        {
        case PAGE_SIZE_TYPE::SMALL:
        {
            ph->thread_free_small(free_idx, p);
            break;
        }
        case PAGE_SIZE_TYPE::NORMAL:
        {
            ph->thread_free_normal(free_idx, p);
            break;
        }
        case PAGE_SIZE_TYPE::BIG:
        {
            ph->thread_free_huge(free_idx, p);
            break;
        }
        default:
        {
            ph->thread_free_unmanage(0, p);
            break;
        }
        }
    }
}

template <typename T>
void PoolByteBufferAllocator<T>::alloc_unmanage(std::uint64_t size, T *&data)
{
    data = new T[size];
}

template <typename T>
void PoolByteBufferAllocator<T>::alloc_small(std::uint64_t index, T *&data)
{
    get_thread_local_page_heap()->alloc_small(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::alloc_normal(std::uint64_t index, T *&data)
{
    get_thread_local_page_heap()->alloc_normal(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::alloc_huge(std::uint64_t index, T *&data)
{
    get_thread_local_page_heap()->alloc_huge(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::free_small(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->free_small(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::free_normal(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->free_normal(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::free_huge(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->free_huge(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::free_unmanage(std::uint64_t size, T *data)
{
    delete[] data;
}

template <typename T>
void PoolByteBufferAllocator<T>::thread_free_small(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->thread_free_small(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::thread_free_normal(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->thread_free_normal(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::thread_free_huge(std::uint64_t index, T *data)
{
    get_thread_local_page_heap()->thread_free_huge(index, data);
}

template <typename T>
void PoolByteBufferAllocator<T>::thread_free_unmanage(std::uint64_t index, T *data)
{
    delete[] data;
}

template <typename T>
SizeClass *PoolByteBufferAllocator<T>::size_class()
{
    return _size_class;
}

template <typename T>
std::shared_ptr<PageHeap<T>> PoolByteBufferAllocator<T>::get_thread_local_page_heap()
{
    static thread_local ThreadPageHeap<T> tph;
    return tph.get();
}
#endif /* __POOL_BYTE_BUFFER_ALLOCATOR_HPP__ */
