#ifndef __LINKED_BLOCKING_QUEUE_HPP__
#define __LINKED_BLOCKING_QUEUE_HPP__

#include <cassert>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <limits>

#include "concurrent_queue/concurrent_queue.hpp"

//ArrayBlockingQueue 底层由链表实现的队列, 默认容量std::numeric_limits<std::size_t>::max()
template <typename T>
class LinkedBlockingQueue : public ConcurrentQueue<T>
{
private:
    class Node
    {
    public:
        Node(T &&ele) : ele(std::forward<T>(ele)) {}
        Node() {}
        T ele;
        Node *next = nullptr;
    };

private:
    std::size_t _cap;
    std::mutex _take_mutex;
    std::mutex _put_mutex;
    std::condition_variable _not_full;
    std::condition_variable _not_empty;
    Node *_head;
    Node *_tail;
    std::atomic<::size_t> _count{0};

public:
    /**
     * @brief 构造函数
     * @param cap 最大容量
     */ 
    LinkedBlockingQueue(std::size_t cap = std::numeric_limits<std::size_t>::max());
    ~LinkedBlockingQueue();
    LinkedBlockingQueue(const LinkedBlockingQueue &) = delete;
    LinkedBlockingQueue &operator=(const LinkedBlockingQueue &) = delete;
    /**
     * @brief 阻塞地将元素的放入队列
     */
    virtual void push(T &&);
    /**
     * @brief 将元素的放入队列, 立即返回
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool try_push(T &&);
    /**
     * @brief 将元素的放入队列
     * @param T 入队元素
     * @return bool 入队是否成功
     */
    virtual bool wait_push(T &&, std::size_t seconds, std::size_t nano_seconds = 0);
    /**
     * @brief 将元素弹出队列
     */
    virtual T &&pop();
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
     * @brief 队列已满
     */
    bool full();
    /**
     * @brief FIFO插入元素
     */
    void insert(T &&ele);
    /**
     * @brief FIFO删除元素
     */
    T &&remove();
};

template <typename T>
LinkedBlockingQueue<T>::LinkedBlockingQueue(std::size_t cap)
    : _cap(cap), _head(new Node()), _tail(_head)
{
    assert(_head == _tail);
}

template <typename T>
LinkedBlockingQueue<T>::~LinkedBlockingQueue()
{
    while (_head->next != nullptr)
    {
        Node *node = _head->next;
        _head->next = node->next;
        delete node;
    }
    delete _head;
}

template <typename T>
void LinkedBlockingQueue<T>::push(T &&ele)
{
    {
        std::unique_lock<std::mutex> lock(_put_mutex);
        _not_full.wait(lock, [this]()
                       { return !full(); });
        this->insert(std::forward<T>(ele));
    }

    _not_empty.notify_one();
}

template <typename T>
bool LinkedBlockingQueue<T>::try_push(T &&ele)
{
    if (_put_mutex.try_lock() && !full())
    {
        this->insert(std::forward<T>(ele));
        _put_mutex.unlock();
        _not_empty.notify_one();
        return true;
    }

    return false;
}

template <typename T>
bool LinkedBlockingQueue<T>::wait_push(T &&ele, std::size_t seconds, std::size_t nano_seconds)
{
    bool res;
    {
        std::unique_lock<std::mutex> lock(_put_mutex);
        std::size_t nonas = seconds * 1e9 + nano_seconds;
        res = _not_full.wait_for(lock, std::chrono::nanoseconds(nonas), [this]()
                                 { return !full(); });
        if (res)
        {
            this->insert(std::forward<T>(ele));
            _not_empty.notify_one();
        }
    }

    return res;
}

template <typename T>
T &&LinkedBlockingQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(_take_mutex);
    _not_empty.wait(lock, [this]()
                    { return !empty(); });

    auto ele = remove();
    _not_full.notify_one();

    return std::forward<T>(ele);
}

template <typename T>
bool LinkedBlockingQueue<T>::try_pop(T &ele)
{
    if (_take_mutex.try_lock() && !empty())
    {
        ele = remove();
        _take_mutex.unlock();
        _not_full.notify_one();
        return true;
    }

    return false;
}

template <typename T>
bool LinkedBlockingQueue<T>::wait_pop(T &ele, std::size_t seconds, std::size_t nano_seconds)
{
    bool res;
    {
        std::unique_lock<std::mutex> lock(_take_mutex);
        std::size_t nonas = seconds * 1e9 + nano_seconds;
        res = _not_empty.wait_for(lock, std::chrono::nanoseconds(nonas), [this]()
                                  { return !empty(); });
        if (res)
        {
            ele = remove();
            _not_full.notify_one();
        }
    }

    return res;
}

template <typename T>
std::size_t LinkedBlockingQueue<T>::size()
{
    return _cap - _count;
}

template <typename T>
std::size_t LinkedBlockingQueue<T>::cap()
{
    return _cap;
}

template <typename T>
bool LinkedBlockingQueue<T>::empty()
{
    return _count == 0;
}

template <typename T>
bool LinkedBlockingQueue<T>::full()
{
    return _count == _cap;
}

template <typename T>
void LinkedBlockingQueue<T>::insert(T &&ele)
{
    Node *node = new Node(std::forward<T>(ele));
    _tail->next = node;
    _tail = _tail->next;
    _count++;
}

template <typename T>
T &&LinkedBlockingQueue<T>::remove()
{
    //这里不能直接将_head->next从链表中取出, 否则_head->next == _tail时, 会将_tail删除(operator delete)
    //应该删除_head, 并将_head更新为_head->next
    Node *h = _head;
    Node *node = h->next;
    _head = node;

    //_head元素置空
    auto ele = std::move(_head->ele);
    delete h;
    _count--;
    return std::forward<T>(ele);
}
#endif // __LINKED_BLOCKING_QUEUE_HPP__