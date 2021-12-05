#ifndef __SELECTOR_HPP__
#define __SELECTOR_HPP__

#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include <cassert>
#include <chrono>
#include <stdexcept>

#include "net/select/selected.hpp"
#include "net/select/selectable.hpp"
#include "net/socket.hpp"

constexpr int SELECTOR_MAX_EVENTS = 1024;

/**
 * @brief epoll封装
 * 
 */
template <typename T>
class Selector
{
public:
    typedef Selectable::native_handle_type native_handle_type;

public:
    Selector();
    ~Selector();
    /**
     * @brief 添加Socket对象加入事件循环
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     * @param attachment 附件
     */
    void add(const Socket &selectable, Selectable::OPCollection ops, T attachment);
    /**
     * @brief 添加Socket对象加入事件循环
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     */
    void add(const Socket &selectable, Selectable::OPCollection ops);

    /**
     * @brief 阻塞等待事件发生
     * 
     * @tparam Container 
     * @tparam Rep 
     * @tparam Period 
     * @param container 
     * @param duration 
     * @return std::size_t 
     */
    template <typename Container, typename Rep, typename Period>
    std::size_t select(Container &container, const std::chrono::duration<Rep, Period> &duration);

    native_handle_type native_handle();

private:
    friend class Selected<T>;
    struct EventData
    {
        Socket *socket;
        T attachment;
    };

    /**
     * @brief 添加Socket对象加入事件循环
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     */
    void add(Socket *selectable, Selectable::OPCollection ops);

    void add(Socket *selectable, Selectable::OPCollection ops, T attachment);
    /**
     * @brief 修改监听事件
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     */
    void mod(EventData *evdata, Selectable::OPCollection ops);
    /**
     * @brief 删除注册的Socket对象, 不要直接使用remove, 
     *        而应该在select函数返回后, 使用Selected::release()自动释放资源
     * 
     * @param selectable 要删除的对象
     */
    void remove(EventData* evdata);
    epoll_event _events[SELECTOR_MAX_EVENTS];
    native_handle_type _native_handler;
};

template <typename T>
Selector<T>::Selector()
{
    _native_handler = ::epoll_create(SELECTOR_MAX_EVENTS);
    assert(_native_handler > 0);
}

template <typename T>
Selector<T>::~Selector()
{
    ::close(_native_handler);
}

template <typename T>
void Selector<T>::add(const Socket &selectable, Selectable::OPCollection ops, T attachment)
{
    return add(new Socket(selectable), ops, attachment);
}

template <typename T>
void Selector<T>::add(const Socket &selectable, Selectable::OPCollection ops)
{
    return add(new Socket(selectable), ops);
}

template <typename T>
template <typename Container, typename Rep, typename Period>
std::size_t Selector<T>::select(Container &container, const std::chrono::duration<Rep, Period> &duration)
{
    static_assert(std::is_same<typename Container::value_type, Selected<T>>::value, "container's type must be Container<Selected>");
    std::chrono::milliseconds timeout = duration;
    int ret = ::epoll_wait(_native_handler, _events, SELECTOR_MAX_EVENTS, timeout.count());
    if (ret == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    container.reserve(ret + container.size());
    for (int i; i < ret; i++)
    {
        EventData *evdata = reinterpret_cast<EventData *>(_events[i].data.ptr);
        container.push_back(Selected<T>(this, evdata, _events[i].events));
    }
    return ret;
}

template <typename T>
typename Selector<T>::native_handle_type Selector<T>::native_handle()
{
    return _native_handler;
}

template <typename T>
void Selector<T>::add(Socket *selectable, Selectable::OPCollection ops, T attachment)
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = new EventData{selectable, attachment};

    if (epoll_ctl(_native_handler, EPOLL_CTL_ADD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

template <typename T>
void Selector<T>::add(Socket *selectable, Selectable::OPCollection ops)
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = new EventData{selectable};

    if (epoll_ctl(_native_handler, EPOLL_CTL_ADD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

template <typename T>
void Selector<T>::mod(EventData *evdata, Selectable::OPCollection ops)
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = evdata;
    if (epoll_ctl(_native_handler, EPOLL_CTL_MOD, evdata->socket->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

template <typename T>
void Selector<T>::remove(EventData* evdata)
{
    if (epoll_ctl(_native_handler, EPOLL_CTL_DEL, evdata->socket->native_handle(), NULL) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

#endif /* __SELECTOR_HPP__ */
