#ifndef __SELECTED_HPP__
#define __SELECTED_HPP__

#include "net/select/selectable.hpp"

namespace net
{
    namespace select
    {
        template <typename T>
        class Selector;
        class Socket;

        /**
         * @brief Selected<T> 仅仅持有Selector<T> *和Selector<T>::Attachment *；不做new和delete
         * 
         * @tparam T 
         */
        template <typename T>
        class Selected
        {
        private:
            Selector<T> *_selector;
            typename Selector<T>::Attachment *_evdata;
            Selectable::OPCollection _op;

        private:
            friend class Selector<T>;
            Selected(Selector<T> *selector, typename Selector<T>::Attachment *evdata, Selectable::OPCollection op);

        public:
            ~Selected();
            Selector<T> &selector();
            Selectable::native_handle_type selectable();
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
        Selected<T>::Selected(Selector<T> *selector, typename Selector<T>::Attachment *evdata, Selectable::OPCollection op)
            : _selector(selector), _evdata(evdata), _op(op) {}

        template <typename T>
        Selected<T>::~Selected() {}

        template <typename T>
        Selector<T> &Selected<T>::selector()
        {
            return *_selector;
        }

        template <typename T>
        Selectable::native_handle_type Selected<T>::selectable()
        {
            return *_evdata->fd;
        }

        template <typename T>
        T &Selected<T>::attachment()
        {
            return _evdata->data;
        }

        template <typename T>
        Selectable::OPCollection Selected<T>::operation()
        {
            return _op;
        }

        template <typename T>
        void Selected<T>::release()
        {
            _selector->del(_evdata->fd, _evdata);
        }

        template <typename T>
        void Selected<T>::select(Selectable::OPCollection ops)
        {
            _selector->mod(_evdata->fd, ops, _evdata);
        }

    } // namespace select
} // namespace net

#endif /* __SELECTED_HPP__ */
