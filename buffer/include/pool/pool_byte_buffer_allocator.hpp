#ifndef __POOL_BYTE_BUFFER_ALLOCATOR_HPP__
#define __POOL_BYTE_BUFFER_ALLOCATOR_HPP__

#include <memory>

#include "byte/byte.hpp"

class PageHeap;
class SizeClass;

class PoolByteBufferAllocator
{
private:
    SizeClass *_size_class;

private:
    PoolByteBufferAllocator();

public:
    PoolByteBufferAllocator(const PoolByteBufferAllocator &) = delete;
    PoolByteBufferAllocator &operator=(const PoolByteBufferAllocator &) = delete;
    PoolByteBufferAllocator(PoolByteBufferAllocator &&) = delete;
    PoolByteBufferAllocator &operator=(PoolByteBufferAllocator &&) = delete;

    static PoolByteBufferAllocator &instance();
    ~PoolByteBufferAllocator();

    /**
     * @brief 申请一块新的size大小的内存, 在缓存为空时使用
     * @param size 内存大小
     * @param data 申请内存指针
     */
    void alloc_unmanage(std::uint64_t size, Byte *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_small(std::uint64_t index, Byte *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_normal(std::uint64_t index, Byte *&data);
    /**
     * @brief 申请size大小的内存
     * @param page_type 请求page类型
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void alloc_huge(std::uint64_t index, Byte *&data);

    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_small(std::uint64_t index, Byte *data);
    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_normal(std::uint64_t index, Byte *data);
    /**
     * @brief 本线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void free_huge(std::uint64_t index, Byte *data);
    /**
     * @brief 本线程返还内存
     * @param size 内存size
     * @param data 申请内存指针
     */
    void free_unmanage(std::uint64_t size, Byte *data);

    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_small(std::uint64_t index, Byte *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_normal(std::uint64_t index, Byte *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_huge(std::uint64_t index, Byte *data);
    /**
     * @brief 由其他线程返还内存
     * @param index 内存在free_list的下标
     * @param data 申请内存指针
     */
    void thread_free_unmanage(std::uint64_t index, Byte *data);

    SizeClass *size_class();

    std::shared_ptr<PageHeap> get_thread_local_page_heap();
};

#endif // __POOL_BYTE_BUFFER_ALLOCATOR_HPP__