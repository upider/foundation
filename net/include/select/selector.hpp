#ifndef __SELECTOR_HPP__
#define __SELECTOR_HPP__

#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include <cassert>
#include <chrono>
#include <stdexcept>

#include "select/selected.hpp"
#include "select/selectable.hpp"
#include "socket/socket.hpp"

constexpr int SELECTOR_MAX_EVENTS = 1024;

/**
 * @brief epoll封装
 * 
 */
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
    friend class Selected;
    /**
     * @brief 添加Socket对象加入事件循环
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     */
    void add(Socket *selectable, Selectable::OPCollection ops);
    /**
     * @brief 修改监听事件
     * 
     * @param selectable 要添加的Socket对象
     * @param ops 要监听的操作
     */
    void mod(Socket *selectable, Selectable::OPCollection ops);
    /**
     * @brief 删除注册的Socket对象, 不要直接使用remove, 
     *        而应该在select函数返回后, 使用Selected::release()自动释放资源
     * 
     * @param selectable 要删除的对象
     */
    void remove(const Socket &selectable);
    void delete_event(epoll_event *ev);
    epoll_event _events[SELECTOR_MAX_EVENTS];
    native_handle_type _native_handler;
    //TODO: timerfd用于select超时，eventfd用于select终止，新的select函数
    native_handle_type _timerfd;
    native_handle_type _eventfd;
};

template <typename Container, typename Rep, typename Period>
std::size_t Selector::select(Container &container, const std::chrono::duration<Rep, Period> &duration)
{
    static_assert(std::is_same<typename Container::value_type, Selected>::value, "container's type must be Container<Selected>");
    std::chrono::milliseconds timeout = duration;
    int ret = ::epoll_wait(_native_handler, _events, SELECTOR_MAX_EVENTS, timeout.count());
    if (ret == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    container.reserve(ret + container.size());
    for (int i; i < ret; i++)
    {
        Socket *st = reinterpret_cast<Socket *>(_events[i].data.ptr);
        container.push_back(Selected(this, st, _events[i].events));
    }
    return ret;
}

#endif /* __SELECTOR_HPP__ */
