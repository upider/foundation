#ifndef __THREAD_LOCAL_PTR_HPP__
#define __THREAD_LOCAL_PTR_HPP__

#include <memory>

/**
 * @brief 持有类型智能指针的thread local, 使用时"thread_local ThreadLocalPtr<A> a;"
 *        这个类的特殊点在于, 某个线程用get()得到指针后, 可以再将指针传给其他线程从而实现一些特殊需求
 */
template <typename T>
class ThreadLocalPtr
{
private:
    std::shared_ptr<T> _ptr;

public:
    template <typename... ARGS>
    ThreadLocalPtr(ARGS &&...args);
    ~ThreadLocalPtr();

    //得到智能指针
    std::shared_ptr<T> get_ptr();
    //得到智能指针的引用, 只应该在ThreadLocalPtr<T>声明的线程中使用
    std::shared_ptr<T>& get_ptr_ref();
};

template <typename T>
template <typename... ARGS>
ThreadLocalPtr<T>::ThreadLocalPtr(ARGS &&...args) : _ptr(std::make_shared<T>(std::forward<ARGS>(args)...))
{
}

template <typename T>
ThreadLocalPtr<T>::~ThreadLocalPtr()
{
}

template <typename T>
std::shared_ptr<T> ThreadLocalPtr<T>::get_ptr()
{
    return _ptr;
}

template<typename T>
std::shared_ptr<T>& ThreadLocalPtr<T>::get_ptr_ref() 
{
    return _ptr;
}

#endif // __THREAD_LOCAL_PTR_HPP__