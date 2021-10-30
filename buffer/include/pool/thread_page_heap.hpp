#ifndef __THREAD_PAGE_HEAP_HPP__
#define __THREAD_PAGE_HEAP_HPP__

#include <memory>

class PageHeap;

class ThreadPageHeap
{
private:
    //防止_page_heap在其他线程使用时被删除，必须使用shared_ptr
    std::shared_ptr<PageHeap> _page_heap;

public:
    ThreadPageHeap();
    ~ThreadPageHeap();

    std::shared_ptr<PageHeap> get();
};

#endif // __THREAD_PAGE_HEAP_HPP__