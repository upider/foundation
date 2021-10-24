#ifndef __PAGE_HEAP_HPP__
#define __PAGE_HEAP_HPP__

#include <atomic>
#include <cinttypes>

#include "byte/byte.hpp"
#include "pool/constant.hpp"

//单线程内存页堆, 多线程要使用thread_local
class PageHeap
{
private:
    // free list node
    union PageNode
    {
        union PageNode *next;
        Byte *data;
    };

private:
    //空闲链表
    // 64B ~ 2048B, 增长为2的幂
    PageNode **_smalls_free = new PageNode *[SMALLS_LEN];
    // 4KB ~ 16MB, 增长为4K
    PageNode **_normals_free = new PageNode *[NORMALS_LEN];
    //_huge > 16MB, 17MB ~ 64MB, 增长为1MB
    PageNode **_huges_free = new PageNode *[HUGES_LEN];

    //回收的空闲链表: 释放的内存形成的空闲链表, 包括本线程释放和其他线程释放
    // 64B ~ 2048B, 增长为2的幂
    std::atomic<PageNode *> *_thread_smalls_free = new std::atomic<PageNode *>[SMALLS_LEN];
    // 4KB ~ 16MB, 增长为4K
    std::atomic<PageNode *> *_thread_normals_free = new std::atomic<PageNode *>[NORMALS_LEN];
    //_huge > 16MB, 17MB ~ 64MB, 增长为1MB
    std::atomic<PageNode *> *_thread_huges_free = new std::atomic<PageNode *>[HUGES_LEN];

private:
    //头节点插入
    static void insert(std::atomic<PageNode *> &head, PageNode *&node);
    //初始化
    static void init(PageNode **&head, std::size_t size);
    static void destory(PageNode **&head, std::size_t size);
    static void init_atomic(std::atomic<PageNode *> *&head, std::size_t size);
    static void destory_atomic(std::atomic<PageNode *> *&head, std::size_t size);

public:
    PageHeap(const PageHeap&) = delete;
    PageHeap& operator=(const PageHeap&) = delete;
    PageHeap();
    ~PageHeap();

    static void alloc(std::uint64_t index, Byte *&data, std::atomic<PageNode *> *&atomic_head, PageHeap::PageNode **&head);
    static void free(std::uint64_t index, Byte *data, PageHeap::PageNode **&head);
    static void free(std::uint64_t index, Byte *data, std::atomic<PageNode *> *&atomic_head);

    void alloc_small(std::uint64_t index, Byte *&data);
    void alloc_normal(std::uint64_t index, Byte *&data);
    void alloc_huge(std::uint64_t index, Byte *&data);
    void alloc_unmanage(std::uint64_t size, Byte *&data);

    void free_small(std::uint64_t index, Byte *data);
    void free_normal(std::uint64_t index, Byte *data);
    void free_huge(std::uint64_t index, Byte *data);
    void free_unmanage(std::uint64_t index, Byte *data);

    void thread_free_small(std::uint64_t index, Byte *data);
    void thread_free_normal(std::uint64_t index, Byte *data);
    void thread_free_huge(std::uint64_t index, Byte *data);
    void thread_free_unmanage(std::uint64_t index, Byte *data);
};

#endif // __PAGE_HEAP_HPP__