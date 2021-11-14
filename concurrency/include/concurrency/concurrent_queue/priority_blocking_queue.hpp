#ifndef __PRIORITY_BLOCKING_QUEUE_HPP__
#define __PRIORITY_BLOCKING_QUEUE_HPP__

#include <queue>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include "concurrency/concurrent_queue/concurrent_queue.hpp"

//PriorityBlockingQueue 多线程优先级队列, 无界
template <typename T, typename Compare = std::less<T>>
class PriorityBlockingQueue : public ConcurrentQueue<T>
{
private:
    std::priority_queue<T, std::vector<T>, Compare> _queue{};
    std::mutex _mutex;
    std::condition_variable _not_empty;

public:
    PriorityBlockingQueue();
    virtual ~PriorityBlockingQueue();
    PriorityBlockingQueue(const PriorityBlockingQueue &) = delete;
    PriorityBlockingQueue &operator=(const PriorityBlockingQueue &) = delete;

    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(T &&);
    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(const T &);
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(T &&);
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(const T &);
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool wait_push(T &&, std::size_t seconds, std::size_t nano_seconds = 0);
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool wait_push(const T &, std::size_t seconds, std::size_t nano_seconds = 0);
    /**
     * @brief 将元素弹出队列
     */
    virtual T pop();
    /**
     * @brief try_pop 弹出队列元素, 立即返回
     *
     * @param value 弹出元素赋值对象
     */
    virtual bool try_pop(T &value);
    /**
     * @brief wait_pop 弹出队列元素
     *
     * @param value 弹出元素赋值对象
     */
    virtual bool wait_pop(T &value, std::size_t seconds, std::size_t nano_seconds = 0);
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
bool PriorityBlockingQueue<T, Compare>::wait_push(T &&ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::unique_lock<std::mutex> lock(_mutex);
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    auto res = _not_empty.wait_for(lock, std::chrono::nanoseconds(nonas));
    if (res == std::cv_status::no_timeout)
    {
        this->insert(std::forward<T>(ele));
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::wait_push(const T &ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::unique_lock<std::mutex> lock(_mutex);
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    auto res = _not_empty.wait_for(lock, std::chrono::nanoseconds(nonas));
    if (res == std::cv_status::no_timeout)
    {
        this->insert(ele);
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Compare>
T PriorityBlockingQueue<T, Compare>::pop()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _not_empty.wait(lock, [this]()
                    { return !empty(); });
    auto ele = this->remove();

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
        ele = this->remove();
        _mutex.unlock();
        return true;
    }
    return false;
}

template <typename T, typename Compare>
bool PriorityBlockingQueue<T, Compare>::wait_pop(T &ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::unique_lock<std::mutex> lock(_mutex);
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    if (_not_empty.wait_for(lock, std::chrono::nanoseconds(nonas), [this]()
                            { return !empty(); }))
    {
        ele = remove();
        return true;
    }
    else
    {
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

#endif // __PRIORITY_BLOCKING_QUEUE_HPP__