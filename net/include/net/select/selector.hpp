#ifndef __SELECTOR_HPP__
#define __SELECTOR_HPP__

#include <unistd.h>
#include <sys/epoll.h>

#include <cstring>
#include <cassert>
#include <chrono>
#include <stdexcept>

#include "net/select/selected.hpp"
#include "net/select/selectable.hpp"

namespace net
{
    namespace select
    {
        constexpr int SELECTOR_MAX_EVENTS = 1024;
        /**
         * @brief epoll封装
         * 
         * @tparam T attachment类型
         */
        template <typename T>
        class Selector
        {
        public:
            typedef int native_handle_type;
            struct Attachment
            {
                native_handle_type fd;
                T data;
            };
            friend class Selected<T>;
            typedef T value_type;

        private:
            epoll_event _events[SELECTOR_MAX_EVENTS];
            native_handle_type _native_handle;

        public:
            Selector();
            ~Selector();
            native_handle_type native_handle();

            void add(native_handle_type fd, Selectable::OPCollection ops, T attachment);
            void add(native_handle_type fd, Selectable::OPCollection ops);
            void mod(native_handle_type fd, Selectable::OPCollection ops, Attachment* attachment);
            void del(native_handle_type fd, Attachment* attachment);

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
        };

        template <typename T>
        Selector<T>::Selector()
        {
            _native_handle = ::epoll_create(SELECTOR_MAX_EVENTS);
            assert(_native_handle > 0);
        }

        template <typename T>
        Selector<T>::~Selector()
        {
            ::close(_native_handle);
        }

        template <typename T>
        typename Selector<T>::native_handle_type Selector<T>::native_handle()
        {
            return _native_handle;
        }

        template <typename T>
        void Selector<T>::add(native_handle_type fd, Selectable::OPCollection ops, T attachment)
        {
            struct epoll_event ev;
            ev.events = ops;
            ev.data.ptr = new Attachment{fd, attachment};
            if (::epoll_ctl(_native_handle, EPOLL_CTL_ADD, fd, &ev) == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
        }

        template <typename T>
        void Selector<T>::add(native_handle_type fd, Selectable::OPCollection ops)
        {
            struct epoll_event ev;
            ev.events = ops;
            //如果没有attachment则设为空
            ev.data.ptr = new Attachment{fd};
            if (::epoll_ctl(_native_handle, EPOLL_CTL_ADD, fd, &ev) == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
        }

        template <typename T>
        void Selector<T>::mod(native_handle_type fd, Selectable::OPCollection ops, Attachment* attachment)
        {
            struct epoll_event ev;
            ev.events = ops;
            ev.data.ptr = attachment;
            if (::epoll_ctl(_native_handle, EPOLL_CTL_MOD, fd, &ev) == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
        }

        template <typename T>
        void Selector<T>::del(native_handle_type fd, Attachment* attachment)
        {
            delete attachment;
            if (::epoll_ctl(_native_handle, EPOLL_CTL_DEL, fd, NULL) == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
        }

        template <typename T>
        template <typename Container, typename Rep, typename Period>
        std::size_t Selector<T>::select(Container &container, const std::chrono::duration<Rep, Period> &duration)
        {
            static_assert(std::is_same<typename Container::value_type, Selected<T>>::value, "container's type must be Container<Selected>");
            std::chrono::milliseconds timeout = duration;
            int ret = ::epoll_wait(_native_handle, _events, SELECTOR_MAX_EVENTS, timeout.count());
            if (ret == -1)
            {
                throw std::runtime_error(strerror(errno));
            }
            for (int i = 0; i < ret; i++)
            {
                Attachment *evdata = reinterpret_cast<Attachment *>(_events[i].data.ptr);
                container.push_back(Selected<T>(this, evdata, _events[i].events));
            }
            return ret;
        }

    } // namespace select
} // namespace net

#endif /* __SELECTOR_HPP__ */
