#include <cassert>
#include <cmath>

#include "pool/page_heap.hpp"

PageHeap::PageHeap() : _thread_id(std::this_thread::get_id())
{
    init(_smalls_free, SMALLS_LEN);
    init(_normals_free, NORMALS_LEN);
    init(_huges_free, HUGES_LEN);

    init_atomic(_thread_smalls_free, SMALLS_LEN);
    init_atomic(_thread_normals_free, NORMALS_LEN);
    init_atomic(_thread_huges_free, HUGES_LEN);
}

PageHeap::~PageHeap()
{
    destory(_smalls_free, SMALLS_LEN);
    destory(_normals_free, NORMALS_LEN);
    destory(_huges_free, HUGES_LEN);

    destory_atomic(_thread_smalls_free, SMALLS_LEN);
    destory_atomic(_thread_normals_free, NORMALS_LEN);
    destory_atomic(_thread_huges_free, HUGES_LEN);
}

std::thread::id PageHeap::thread_id() 
{
    return _thread_id;
}

void PageHeap::init(PageNode **&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        head[i] = nullptr;
    }
}

void PageHeap::destory(PageNode **&head, std::size_t size)
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

void PageHeap::init_atomic(std::atomic<PageNode *> *&head, std::size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        head[i].store(nullptr);
    }
}

void PageHeap::destory_atomic(std::atomic<PageNode *> *&head, std::size_t size)
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

void PageHeap::alloc(std::uint64_t index, Byte *&data, std::atomic<PageNode *> *&atomic_head, PageHeap::PageNode **&head)
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

void PageHeap::free(std::uint64_t index, Byte *data, std::atomic<PageNode *> *&atomic_head)
{
    auto node = (PageNode *)data;
    node->next = nullptr;
    insert(atomic_head[index], node);
}

void PageHeap::free(std::uint64_t index, Byte *data, PageHeap::PageNode **&head)
{
    PageNode *node = (PageNode *)data;
    node->next = head[index];
    head[index] = node;
}

void PageHeap::alloc_small(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_smalls_free, _smalls_free);
}

void PageHeap::alloc_normal(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_normals_free, _normals_free);
}

void PageHeap::alloc_huge(std::uint64_t index, Byte *&data)
{
    alloc(index, data, _thread_huges_free, _huges_free);
}

void PageHeap::alloc_unmanage(std::uint64_t size, Byte *&data) 
{
    data = new Byte[size];
}

void PageHeap::free_small(std::uint64_t index, Byte *data)
{
    free(index, data, _smalls_free);
}

void PageHeap::free_normal(std::uint64_t index, Byte *data)
{
    free(index, data, _normals_free);
}

void PageHeap::free_huge(std::uint64_t index, Byte *data)
{
    free(index, data, _huges_free);
}

void PageHeap::free_unmanage(std::uint64_t index, Byte *data) 
{
    delete[] data;
}

void PageHeap::thread_free_small(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_smalls_free);
}

void PageHeap::thread_free_normal(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_normals_free);
}

void PageHeap::thread_free_huge(std::uint64_t index, Byte *data)
{
    free(index, data, _thread_huges_free);
}

void PageHeap::thread_free_unmanage(std::uint64_t index, Byte *data) 
{
    delete[] data;
}

void PageHeap::insert(std::atomic<PageNode *> &head, PageNode *&node)
{
    do
    {
        node->next = head;
    } while (!head.compare_exchange_weak(node->next, node));
}