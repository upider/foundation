#ifndef __POOL_BYTE_BUFFER_HPP__
#define __POOL_BYTE_BUFFER_HPP__

#include <memory>
#include <thread>

#include "buffer/byte_buffer.hpp"
#include "byte/byte.hpp"
#include "pool/constant.hpp"

class PoolByteBufferAllocator;
class PageHeap;

class PoolByteBuffer : public ByteBuffer
{
private:
    PoolByteBufferAllocator &_pbba;

    std::weak_ptr<PageHeap> _page_heap_wptr;

    // page 类型
    PAGE_SIZE_TYPE _page_type;
    //空闲链表index
    std::uint64_t _free_idx;
    //容量
    std::uint64_t _cap;
    //申请内存时的线程
    std::thread::id _thread_id;
    //内存
    Byte *_data = nullptr;
    //使用大小
    std::size_t _size = 0;

private:
    void recycle();

public:
    PoolByteBuffer(std::size_t size);
    ~PoolByteBuffer();

    std::size_t cap();
    std::size_t size();
    Byte *data();
};

#endif // __POOL_BYTE_BUFFER_HPP__