#ifndef __THREAD_PAGE_HEAP_HPP__
#define __THREAD_PAGE_HEAP_HPP__

#include <memory>

class PageHeap;

class ThreadPageHeap
{
private:
    std::shared_ptr<PageHeap> _page_heap;

public:
    ThreadPageHeap();
    ~ThreadPageHeap();

    std::shared_ptr<PageHeap> get();
};

#endif // __THREAD_PAGE_HEAP_HPP__