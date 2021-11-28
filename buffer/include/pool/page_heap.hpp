#ifndef __PAGE_HEAP_HPP__
#define __PAGE_HEAP_HPP__

#include <thread>
#include <atomic>
#include <cinttypes>

#include "byte/byte.hpp"
#include "pool/constant.hpp"

template<typename T>
class PageHeap
{
private:
    // free list node
    union PageNode
    {
        union PageNode *next;
        T *data;
    };

private:
    //所属线程id
    std::thread::id _thread_id;

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

    //返回所属线程id
    std::thread::id thread_id();

    static inline void alloc(std::uint64_t index, Byte *&data, std::atomic<PageNode *> *&atomic_head, PageHeap<T>::PageNode **&head);
    static inline void free(std::uint64_t index, Byte *data, PageHeap<T>::PageNode **&head);
    static inline void free(std::uint64_t index, Byte *data, std::atomic<PageNode *> *&atomic_head);

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

template<typename T>
PageHeap<T>::PageHeap() : _thread_id(std::this_thread::get_id())
{
    init(_smalls_free, SMALLS_LEN);
    init(_normals_free, NORMALS_LEN);
    init(_huges_free, HUGES_LEN);

    init_atomic(_thread_smalls_free, SMALLS_LEN);
    init_atomic(_thread_normals_free, NORMALS_LEN);
    init_atomic(_thread_huges_free, HUGES_LEN);
}

template<typename T>
PageHeap<T>::~PageHeap()
{
    destory(_smalls_free, SMALLS_LEN);
    destory(_normals_free, NORMALS_LEN);
    destory(_huges_free, HUGES_LEN);

    destory_atomic(_thread_smalls_free, SMALLS_LEN);
    destory_atomic(_thread_normals_free, NORMALS_LEN);
    destory_atomic(_thread_huges_free, HUGES_LEN);
}

template<typename T>
std::thread::id PageHeap<T>::thread_id() 
{
    return _thread_id;
}

template<typename T>
void PageHeap<T>::init(PageNode **&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        head[i] = nullptr;
    }
}

template<typename T>
void PageHeap<T>::destory(PageNode **&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        PageNode * node0 = head[i];
        while (node0 != nullptr)
        {
            PageNode *node = node0->next;
            delete[] node0;
            node0 = node;
        }
    }
    delete[] head;
}

template<typename T>
void PageHeap<T>::init_atomic(std::atomic<PageNode *> *&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        head[i].store(nullptr);
    }
}

template<typename T>
void PageHeap<T>::destory_atomic(std::atomic<PageNode *> *&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        auto node0 = head[i].load();
        while (node0 != nullptr)
        {
            PageNode *node = node0->next;
            delete[] node0;
            node0 = node;
        }
    }
    delete[] head;
}

template<typename T>
void PageHeap<T>::alloc(std::uint64_t index, Byte *&data, std::atomic<PageNode *> *&atomic_head, PageHeap<T>::PageNode **&head)
{
    PageNode *node = head[index];
    if (node != nullptr)
    {
        //空闲链表不为空
        head[index] = node->next;
    }
    else
    {
        //空闲链表为空
        //将"回收的空闲链表" 与 "空闲链表" 互换, 并再次尝试从"空闲链表"分配
        //若再次分配失败, 则从系统申请
        PageNode *old_head;
        do
        {
            old_head = atomic_head->load();
        } while (!atomic_head->compare_exchange_weak(old_head, head[index]));
        node = head[index];
    }
    // data的结果可能为空
    // data = (node == nullptr) ? nullptr : (Byte *)node;
    data = (Byte *)node;
}

template<typename T>
void PageHeap<T>::free(std::uint64_t index, Byte *data, std::atomic<PageNode *> *&atomic_head)
{
    auto node = (PageNode *)data;
    node->next = nullptr;
    insert(atomic_head[index], node);
}

template<typename T>
void PageHeap<T>::free(std::uint64_t index, Byte *data, PageHeap<T>::PageNode **&head)
{
    PageNode *node = (PageNode *)data;
    node->next = head[index];
    head[index] = node;
}

template<typename T>
void PageHeap<T>::alloc_small(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_smalls_free, _smalls_free);
}

template<typename T>
void PageHeap<T>::alloc_normal(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_normals_free, _normals_free);
}

template<typename T>
void PageHeap<T>::alloc_huge(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_huges_free, _huges_free);
}

template<typename T>
void PageHeap<T>::alloc_unmanage(std::uint64_t size, Byte *&data) 
{
    data = new Byte[size];
}

template<typename T>
void PageHeap<T>::free_small(std::uint64_t index, Byte *data)
{
    free(index, data, _smalls_free);
}

template<typename T>
void PageHeap<T>::free_normal(std::uint64_t index, Byte *data)
{
    free(index, data, _normals_free);
}

template<typename T>
void PageHeap<T>::free_huge(std::uint64_t index, Byte *data)
{
    free(index, data, _huges_free);
}

template<typename T>
void PageHeap<T>::free_unmanage(std::uint64_t index, Byte *data) 
{
    delete[] data;
}

template<typename T>
void PageHeap<T>::thread_free_small(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_smalls_free);
}

template<typename T>
void PageHeap<T>::thread_free_normal(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_normals_free);
}

template<typename T>
void PageHeap<T>::thread_free_huge(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_huges_free);
}

template<typename T>
void PageHeap<T>::thread_free_unmanage(std::uint64_t index, Byte *data) 
{
    delete[] data;
}

template<typename T>
void PageHeap<T>::insert(std::atomic<PageNode *> &head, PageNode *&node)
{
    do
    {
        node->next = head;
    } while (!head.compare_exchange_weak(node->next, node));
}

#endif // __PAGE_HEAP_HPP__