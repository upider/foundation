#ifndef __PRIORITY_QUEUE_HPP__
#define __PRIORITY_QUEUE_HPP__

#include <algorithm>
#include <queue>

//优先级队列，增加remove方法
template <typename T>
class PriorityQueue : public std::priority_queue<T, std::vector<T>, std::greater<T>>
{
public:
    PriorityQueue();
    ~PriorityQueue();
    bool remove(const T &value);
};

template <typename T>
PriorityQueue<T>::PriorityQueue()
{
}

template <typename T>
PriorityQueue<T>::~PriorityQueue() {}

template <typename T>
bool PriorityQueue<T>::remove(const T &value)
{
    auto it = std::find(this->c.begin(), this->c.end(), value);
    if (it != this->c.end())
    {
        this->c.erase(it);
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
        return true;
    }
    return false;
}

#endif // __PRIORITY_QUEUE_HPP__