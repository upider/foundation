#ifndef __PRIORITY_BLOCKING_QUEUE_HPP__
#define __PRIORITY_BLOCKING_QUEUE_HPP__

#include <queue>
#include <chrono>
#include <condition_variable>
#include <mutex>

//PriorityBlockingQueue 多线程优先级队列, 无界
template <typename T, typename Container = std::vector<T>, typename Compare = std::less<T>>
class PriorityBlockingQueue : public ConcurrentQueue<T>
{
private:
    std::priority_queue<T, Container, Compare> _queue;
    std::timed_mutex _mutex;
    std::condition_variable _not_full;
    std::condition_variable _not_empty;

public:
    PriorityBlockingQueue();
    ~PriorityBlockingQueue();
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

template <typename T, typename Container, typename Compare>
PriorityBlockingQueue<T, Container, Compare>::PriorityBlockingQueue()
{
}

template <typename T, typename Container, typename Compare>
PriorityBlockingQueue<T, Container, Compare>::~PriorityBlockingQueue()
{
}

template <typename T, typename Container, typename Compare>
void PriorityBlockingQueue<T, Container, Compare>::push(T &&ele)
{
    _mutex.lock();
    this->insert(std::forward<T>(ele));
    _mutex.unlock();

    _not_empty.notify_one();
}

template <typename T, typename Container, typename Compare>
void PriorityBlockingQueue<T, Container, Compare>::push(const T &ele)
{
    _mutex.lock();
    this->insert(ele);
    _mutex.unlock();

    _not_empty.notify_one();
}

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::try_push(T &&ele)
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

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::try_push(const T &ele)
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

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::wait_push(T &&ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    if (_mutex.try_lock_for(std::chrono::nanoseconds(nonas)))
    {
        this->insert(std::forward<T>(ele));
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::wait_push(const T &ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    if (_mutex.try_lock_for(std::chrono::nanoseconds(nonas)))
    {
        this->insert(ele);
        _mutex.unlock();
        _not_empty.notify_one();
        return true;
    }
    return false;
}

template <typename T, typename Container, typename Compare>
T PriorityBlockingQueue<T, Container, Compare>::pop()
{
    _mutex.lock();
    auto ele = this->remove();
    _mutex.unlock();

    return ele;
}

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::try_pop(T &ele)
{
    if (_mutex.try_lock())
    {
        ele = this->remove();
        _mutex.unlock();
        return true;
    }
    return false;
}

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::wait_pop(T &ele, std::size_t seconds, std::size_t nano_seconds)
{
    std::size_t nonas = seconds * 1e9 + nano_seconds;
    if (_mutex.try_lock_for(std::chrono::nanoseconds(nonas)))
    {
        ele = this->remove();
        _mutex.unlock();
        return true;
    }
    return false;
}

template <typename T, typename Container, typename Compare>
std::size_t PriorityBlockingQueue<T, Container, Compare>::size()
{
    return _queue.size();
}

template <typename T, typename Container, typename Compare>
std::size_t PriorityBlockingQueue<T, Container, Compare>::cap()
{
    return std::numeric_limits<std::size_t>::max();
}

template <typename T, typename Container, typename Compare>
bool PriorityBlockingQueue<T, Container, Compare>::empty()
{
    return _queue.empty();
}

template <typename T, typename Container, typename Compare>
void PriorityBlockingQueue<T, Container, Compare>::insert(T &&ele)
{
    _queue.push(std::forward<T>(ele));
}

template <typename T, typename Container, typename Compare>
void PriorityBlockingQueue<T, Container, Compare>::insert(const T &ele)
{
    _queue.push(ele);
}

template <typename T, typename Container, typename Compare>
T PriorityBlockingQueue<T, Container, Compare>::remove()
{
    auto ele = _queue.top();
    _queue.pop();
    return std::forward<T>(ele);
}

#endif // __PRIORITY_BLOCKING_QUEUE_HPP__