#ifndef __DELAY_QUEUE_HPP__
#define __DELAY_QUEUE_HPP__

#include <utility>
#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "concurrency/mutex/null_mutex.hpp"

template <typename T, typename Clock = std::chrono::system_clock>
class DelayQueue
{
public:
    typedef T value_type;

    using time_point = std::chrono::time_point<Clock>;
    using time_pair = std::pair<T, time_point>;
    using Compare = struct
    {
    public:
        bool operator()(const time_pair &tp1, const time_pair &tp2)
        {
            return tp1.second > tp2.second;
        }
    };

private:
    std::timed_mutex _mutex;
    std::condition_variable_any _cv;
    std::priority_queue<time_pair, std::vector<time_pair>, Compare> _queue;

public:
    DelayQueue();
    ~DelayQueue();

    /**
     * @brief 阻塞地将元素的放入队列
     * 
     * @param ele 入队元素
     * @param tp time_point 超时时间点
     */
    void push(const time_point& tp, T &&ele);

    /**
     * @brief 阻塞地将元素的放入队列
     * 
     * @param ele 入队元素
     * @param tp time_point 超时时间点
     */
    void push(const time_point& tp, const T &ele);
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    bool try_push(const time_point& tp, T &&ele);
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    bool try_push(const time_point& tp, const T &ele);

    template <class Rep, class Period>
    bool wait_push(const time_point& tp, const T &ele, const std::chrono::duration<Rep, Period> &wait_duration);

    template <class Rep, class Period>
    bool wait_push(const time_point& tp, T &&ele, const std::chrono::duration<Rep, Period> &wait_duration);

    /**
     * @brief 将元素弹出队列
     */
    T pop();
    /**
     * @brief try_pop 弹出队列元素, 立即返回
     *
     * @param value 弹出元素赋值对象
     */
    bool try_pop(T &value);

    template <class Rep, class Period>
    bool wait_pop(T &ele, const std::chrono::duration<Rep, Period> &wait_duration);

    template <class Rep, class Period>
    void push(const std::chrono::duration<Rep, Period> &delay, T &&ele);
    template <class Rep, class Period>
    void push(const std::chrono::duration<Rep, Period> &delay, const T &ele);
    template <class Rep, class Period>
    bool try_push(const std::chrono::duration<Rep, Period> &delay, T &&ele);
    template <class Rep, class Period>
    bool try_push(const std::chrono::duration<Rep, Period> &delay, const T &);
    template <class Rep, class Period, class WRep, class WPeriod>
    bool wait_push(const std::chrono::duration<Rep, Period> &delay, const T &ele, const std::chrono::duration<WRep, WPeriod> &wait_duration);
    template <class Rep, class Period, class WRep, class WPeriod>
    bool wait_push(const std::chrono::duration<Rep, Period> &delay, T &&ele, const std::chrono::duration<WRep, WPeriod> &wait_duration);
    /**
     * @brief 返回队列大小
     */
    std::size_t size();
    /**
     * @brief 返回队列容量
     */
    std::size_t cap();
    /**
     * @brief 返回队列大小
     */
    bool empty();
};

template <typename T, typename Clock>
DelayQueue<T, Clock>::DelayQueue()
{
}

template <typename T, typename Clock>
DelayQueue<T, Clock>::~DelayQueue()
{
}

template <typename T, typename Clock>
void DelayQueue<T, Clock>::push(const time_point& tp, T &&ele)
{
    _mutex.lock();
    _queue.emplace(std::forward<T>(ele), tp);
    _mutex.unlock();
    _cv.notify_all();
}

template <typename T, typename Clock>
void DelayQueue<T, Clock>::push(const time_point& tp, const T &ele)
{
    _mutex.lock();
    _queue.emplace(ele, tp);
    _mutex.unlock();
    _cv.notify_all();
}

template <typename T, typename Clock>
bool DelayQueue<T, Clock>::try_push(const time_point& tp, T &&ele)
{
    if (_mutex.try_lock())
    {
        _queue.emplace(std::forward<T>(ele), tp);
        _mutex.unlock();
        _cv.notify_all();
        return true;
    }
    return false;
}

template <typename T, typename Clock>
bool DelayQueue<T, Clock>::try_push(const time_point& tp, const T &ele)
{
    if (_mutex.try_lock())
    {
        _queue.emplace(ele, tp);
        _mutex.unlock();
        _cv.notify_all();
        return true;
    }
    return false;
}

template <typename T, typename Clock>
template <class Rep, class Period>
bool DelayQueue<T, Clock>::wait_push(const time_point& tp, const T &ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    bool m = _mutex.try_lock_for(wait_duration);
    if (m)
    {
        _queue.emplace(ele, tp);
        _mutex.unlock();
    }
    return m;
}

template <typename T, typename Clock>
template <class Rep, class Period>
bool DelayQueue<T, Clock>::wait_push(const time_point& tp, T &&ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    bool m = _mutex.try_lock_for(wait_duration);
    if (m)
    {
        _queue.emplace(std::forward<T>(ele), tp);
        _mutex.unlock();
    }
    return m;
}

template <typename T, typename Clock>
T DelayQueue<T, Clock>::pop()
{
    while (true)
    {
        _mutex.lock();
        if (!empty())
        {
            auto p = _queue.top().second;
            _mutex.unlock();
            NullMutex lock;
            if (_cv.wait_until(lock, p) == std::cv_status::timeout)
            {
                _mutex.lock();
                if (p == _queue.top().second)
                {
                    T ele = _queue.top().first;
                    _queue.pop();
                    _mutex.unlock();
                    return ele;
                }
                else
                {
                    _mutex.unlock();
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
            _mutex.unlock();
            NullMutex lock;
            _cv.wait(lock);
        }
    }
}

template <typename T, typename Clock>
bool DelayQueue<T, Clock>::try_pop(T &value)
{
    if (_mutex.try_lock())
    {
        if (!empty())
        {
            auto p = _queue.top().second;
            _mutex.unlock();
            NullMutex lock;
            if (_cv.wait_until(lock, p) == std::cv_status::timeout)
            {
                if (_mutex.try_lock())
                {
                    if (p == _queue.top().second)
                    {
                        value = _queue.top().first;
                        _queue.pop();
                        _mutex.unlock();
                        return true;
                    }
                    _mutex.unlock();
                }
                return false;
            }
            else
            {
                return try_pop(value);
            }
        }
        else
        {
            _mutex.unlock();
            return false;
        }
    }
    return false;
}

template <typename T, typename Clock>
template <class Rep, class Period>
bool DelayQueue<T, Clock>::wait_pop(T &ele, const std::chrono::duration<Rep, Period> &wait_duration)
{
    if (_mutex.try_lock_for(wait_duration))
    {
        if (!empty())
        {
            auto p = _queue.top().second;
            _mutex.unlock();
            NullMutex lock;
            if (_cv.wait_until(lock, p) == std::cv_status::timeout)
            {
                if (_mutex.try_lock())
                {
                    if (p == _queue.top().second)
                    {
                        ele = _queue.top().first;
                        _queue.pop();
                        _mutex.unlock();
                        return true;
                    }
                    _mutex.unlock();
                }
                return false;
            }
            else
            {
                return try_pop(ele);
            }
        }
        else
        {
            _mutex.unlock();
            return false;
        }
    }
    return false;
}

template <typename T, typename Clock>
template <class Rep, class Period>
void DelayQueue<T, Clock>::push(const std::chrono::duration<Rep, Period> &delay, T &&ele)
{
    return push(std::forward<>(ele), Clock::now() + delay);
}

template <typename T, typename Clock>
template <class Rep, class Period>
void DelayQueue<T, Clock>::push(const std::chrono::duration<Rep, Period> &delay, const T &ele)
{
    return push(ele, Clock::now() + delay);
}

template <typename T, typename Clock>
template <class Rep, class Period>
bool DelayQueue<T, Clock>::try_push(const std::chrono::duration<Rep, Period> &delay, T &&ele)
{
    return try_push(std::forward<>(ele), Clock::now() + delay);
}

template <typename T, typename Clock>
template <class Rep, class Period>
bool DelayQueue<T, Clock>::try_push(const std::chrono::duration<Rep, Period> &delay, const T &ele)
{
    return try_push(ele, Clock::now() + delay);
}

template <typename T, typename Clock>
template <class Rep, class Period, class WRep, class WPeriod>
bool DelayQueue<T, Clock>::wait_push(const std::chrono::duration<Rep, Period> &delay, const T &ele, const std::chrono::duration<WRep, WPeriod> &wait_duration)
{
    return wait_push(ele, Clock::now() + delay, wait_duration);
}

template <typename T, typename Clock>
template <class Rep, class Period, class WRep, class WPeriod>
bool DelayQueue<T, Clock>::wait_push(const std::chrono::duration<Rep, Period> &delay, T &&ele, const std::chrono::duration<WRep, WPeriod> &wait_duration)
{
    return wait_push(std::forward<>(ele), Clock::now() + delay, wait_duration);
}

template <typename T, typename Clock>
std::size_t DelayQueue<T, Clock>::size()
{
    return _queue.size();
}

template <typename T, typename Clock>
std::size_t DelayQueue<T, Clock>::cap()
{
    return std::numeric_limits<std::size_t>::max();
}

template <typename T, typename Clock>
bool DelayQueue<T, Clock>::empty()
{
    return _queue.empty();
}

#endif /* __DELAY_QUEUE_HPP__ */
