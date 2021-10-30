#ifndef __CONCURRENT_QUEUE_HPP__
#define __CONCURRENT_QUEUE_HPP__

//FIFO队列
template <typename T>
class ConcurrentQueue
{
public:
    ConcurrentQueue();
    virtual ~ConcurrentQueue();

public:
    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(T &&) = 0;
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(T &&) = 0;
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool wait_push(T &&, std::size_t seconds, std::size_t nano_seconds) = 0;
    /**
     * @brief 将元素弹出队列
     */
    virtual T &&pop() = 0;
    /**
     * @brief try_pop 弹出队列元素, 立即返回
     *
     * @param value 弹出元素赋值对象
     */
    virtual bool try_pop(T &value) = 0;
    /**
     * @brief wait_pop 弹出队列元素
     *
     * @param value 弹出元素赋值对象
     */
    virtual bool wait_pop(T &value, std::size_t seconds, std::size_t nano_seconds) = 0;
    /**
     * @brief 返回队列大小
     */
    virtual std::size_t size() = 0;
    /**
     * @brief 返回队列容量
     */
    virtual std::size_t cap() = 0;
    /**
     * @brief 返回队列大小
     */
    virtual bool empty() = 0;
};

template <typename T>
ConcurrentQueue<T>::ConcurrentQueue()
{
}

template <typename T>
ConcurrentQueue<T>::~ConcurrentQueue()
{
}

#endif // __CONCURRENT_QUEUE_HPP__