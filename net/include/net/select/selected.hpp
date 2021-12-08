#ifndef __SELECTED_HPP__
#define __SELECTED_HPP__

#include "net/select/selectable.hpp"

namespace net
{
    template <typename T>
    class Selector;
    class Socket;

    template <typename T>
    class Selected
    {
    private:
        Selector<T> *_selector;
        typename Selector<T>::EventData *_evdata;
        Selectable::OPCollection _op;

    private:
        friend class Selector<T>;
        Selected(Selector<T> *selector, typename Selector<T>::EventData *evdata, Selectable::OPCollection op);

    public:
        ~Selected();
        Selector<T> &selector();
        Socket &selectable();
        T &attachment();
        /**
     * @brief 返回可执行的Selectable操作集合
     * 
     * @return Selectable::OperationCollection 可执行操作集合
     */
        Selectable::OPCollection operation();
        /**
     * @brief 释放持有的Selectable对象并回收相应资源
     */
        void release();
        /**
     * @brief 重新将持有的Selectable对象放回select循环
     * 
     * @param ops 监听的selectable操作集合
     */
        void select(Selectable::OPCollection ops);
    };

    template <typename T>
    Selected<T>::Selected(Selector<T> *selector, typename Selector<T>::EventData *evdata, Selectable::OPCollection op)
        : _selector(selector), _evdata(evdata), _op(op) {}

    template <typename T>
    Selected<T>::~Selected() {}

    template <typename T>
    Selector<T> &Selected<T>::selector()
    {
        return *_selector;
    }

    template <typename T>
    Socket &Selected<T>::selectable()
    {
        return *_evdata->socket;
    }

    template <typename T>
    T &Selected<T>::attachment()
    {
        return _evdata->attachment;
    }

    template <typename T>
    Selectable::OPCollection Selected<T>::operation()
    {
        return _op;
    }

    template <typename T>
    void Selected<T>::release()
    {
        _selector->remove(_evdata);
        _evdata->socket->close();
        delete _evdata;
        _evdata = nullptr;
    }

    template <typename T>
    void Selected<T>::select(Selectable::OPCollection ops)
    {
        _selector->mod(_evdata, ops);
    }
} // namespace net

#endif /* __SELECTED_HPP__ */
