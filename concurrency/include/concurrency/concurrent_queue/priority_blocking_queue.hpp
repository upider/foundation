#ifndef __PRIORITY_BLOCKING_QUEUE_HPP__
#define __PRIORITY_BLOCKING_QUEUE_HPP__

#include <queue>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "concurrency/concurrent_queue/concurrent_queue.hpp"

/**
 * @brief 多线程优先级队列, 无界
 * 
 * @tparam T 元素类型
 * @tparam Compare 排序方式默认std::less<int>
 */
template <typename T, typename Compare = std::less<T>>
class PriorityBlockingQueue
{
private:
    std::priority_queue<T, std::vector<T>, Compare> _queue;
    std::timed_mutex _mutex;
    std::condition_variable_any _not_empty;

public:
    typedef T value_type;
    PriorityBlockingQueue();
    virtual ~PriorityBlockingQueue();
    PriorityBlockingQueue(const PriorityBlockingQueue &) = delete;
    PriorityBlockingQueue &operator=(const PriorityBlockingQueue &) = delete;

    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(T &&ele);
    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(const T &ele);
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(T &&ele);
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(const T &ele);
    /**
     * @brief 等待地将元素放进队列
     * 
     * @tparam Rep 刻度数的算术类型
     * @tparam Period 滴答周期
     * @param ele 入队元素
     * @param wait_duration 等待时间
     * @return true 入队成功
     * @return false 入队失败
     */
    template <class Rep, class Period>
    bool wait_push(const T &ele, const std::chrono::duration<Rep, Period> &wait_duration);

    /**
     * @brief 等待地将元素放进队列
     * 
     * @tparam Rep 刻度数的算术类型
     * @tparam Period 滴答周期
     * @param ele 入队元素
     * @param wait_duration 等待时间
     * @return true 入队成功
     * @return false 入队失败
     */
    template <class Rep, class Period>
    bool wait_push(T &&ele, const std::chrono::duration<Rep, Period> &wait_duration);
    /**
     * @brief 将元素弹出队列
     */
    virtual T pop();
    /**
     * @brief try_pop 弹出队列元素, 立即返回
     *
     * @param value 弹出元素赋值对象
     */
    virtual bool try_pop(T &ele);

    /**
     * @brief 将元素弹出队列
     * 
     * @tparam Rep 刻度数的算术类型
     * @tparam Period 滴答周期
     * @param ele 出队元素
     * @param wait_duration 等待时间
     * @return true 入队成功
     * @return false 入队失败
     */
    template <class Rep, class Period>
    bool wait_pop(T &ele, const std::chrono::duration<Rep, Period> &wait_duration);

    /**
     * @brief 返回队列大小
     */
    virtual std::size_t size();
    /**
     * @brief 返回队列容量
     */
    virtual std::size_t cap();
    /**
     * @brief 返回队列大小
     */
    virtual bool empty();

private:
    /**
     * @brief FIFO插入元素
     */
    void insert(T &&ele);
    /**
     * @brief FIFO插入元素
     */
    void insert(const T &ele);
    /**
     * @brief FIFO删除元素
     */
    T remove();
};

template <typename T, typename Compare>
PriorityBlockingQueue<T, Compare>::PriorityBlockingQueue() {}

template <typename T, typename Compare>
PriorityBlockingQueue<T, Compare>::~PriorityBlockingQueue() {}

template <typename T, typename Compare>
void PriorityBlockingQueue<T, Compare>::push(T &&ele)
{
    _mutex.lock();
    this->insert(std::forward<T>(ele));
    _mutex.unlock();
    _not_empty.notify_one();
}

template <typename T, typename Compare>
void PriorityBlockingQueue<T, Compare>::push(const T &ele)
{
    _mutex.lock();
    this->insert(ele);
    _mutex.unlock();
    _not_empty.notify_one();
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::try_push(T &&ele)
{
    if (_mutex.try_lock())
    {
        this->insert(std::forward<T>(ele));
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::try_push(const T &ele)
{
    if (_mutex.try_lock())
    {
        this->insert(ele);
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Compare>
template <class Rep, class Period>
bool PriorityBlockingQueue<T, Compare>::wait_push(const T &ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    bool res = _mutex.try_lock_for(wait_duration);
    if (res)
    {
        this->insert(ele);
        _mutex.unlock();
        _not_empty.notify_one();
    }
    return res;
}

template <typename T, typename Compare>
template <class Rep, class Period>
bool PriorityBlockingQueue<T, Compare>::wait_push(T &&ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    bool res = _mutex.try_lock_for(wait_duration);
    if (res)
    {
        this->insert(ele);
        _mutex.unlock();
        _not_empty.notify_one();
    }
    return res;
}

template <typename T, typename Compare>
T PriorityBlockingQueue<T, Compare>::pop()
{
    _not_empty.wait(_mutex, [this]()
                    { return !empty(); });
    auto ele = this->remove();
    _mutex.unlock();
    return ele;
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::try_pop(T &ele)
{
    if (_mutex.try_lock())
    {
        if (empty())
        {
            _mutex.unlock();
            return false;
        }
        else
        {
            ele = this->remove();
            _mutex.unlock();
            return true;
        }
    }
    else
    {
        return false;
    }
}

template <typename T, typename Compare>
template <class Rep, class Period>
bool PriorityBlockingQueue<T, Compare>::wait_pop(T &ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (_not_empty.wait_for(_mutex, wait_duration, [this]()
                            { return !empty(); }))
    {
        ele = remove();
        _mutex.unlock();
        return true;
    }
    else
    {
        _mutex.unlock();
        return false;
    }
}

template <typename T, typename Compare>
std::size_t PriorityBlockingQueue<T, Compare>::size()
{
    return _queue.size();
}

template <typename T, typename Compare>
std::size_t PriorityBlockingQueue<T, Compare>::cap()
{
    return std::numeric_limits<std::size_t>::max();
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::empty()
{
    return _queue.empty();
}

template <typename T, typename Compare>
void PriorityBlockingQueue<T, Compare>::insert(T &&ele)
{
    _queue.push(std::forward<T>(ele));
}

template <typename T, typename Compare>
void PriorityBlockingQueue<T, Compare>::insert(const T &ele)
{
    _queue.push(ele);
}

template <typename T, typename Compare>
T PriorityBlockingQueue<T, Compare>::remove()
{
    auto ele = _queue.top();
    _queue.pop();
    return ele;
}

#endif /* __PRIORITY_BLOCKING_QUEUE_HPP__ */
