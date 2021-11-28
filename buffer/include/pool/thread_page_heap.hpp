#ifndef __THREAD_PAGE_HEAP_HPP__
#define __THREAD_PAGE_HEAP_HPP__

#include <memory>

template<typename T>
class PageHeap;

template<typename T>
class ThreadPageHeap
{
private:
    //防止_page_heap在其他线程使用时被删除，必须使用shared_ptr
    std::shared_ptr<PageHeap<T>> _page_heap;

public:
    ThreadPageHeap();
    ~ThreadPageHeap();

    std::shared_ptr<PageHeap<T>> get();
};

template<typename T>
ThreadPageHeap<T>::ThreadPageHeap() : _page_heap(std::make_shared<PageHeap<T>>()) {}

template<typename T>
ThreadPageHeap<T>::~ThreadPageHeap() {}

template<typename T>
std::shared_ptr<PageHeap<T>> ThreadPageHeap<T>::get()
{
    return _page_heap;
}
#endif /* __THREAD_PAGE_HEAP_HPP__ */
