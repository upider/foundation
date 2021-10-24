#include "pool/page_heap.hpp"
#include "pool/thread_page_heap.hpp"

ThreadPageHeap::ThreadPageHeap() : _page_heap(std::make_shared<PageHeap>())
{
}

ThreadPageHeap::~ThreadPageHeap()
{
}

std::shared_ptr<PageHeap> ThreadPageHeap::get()
{
    return _page_heap;
}