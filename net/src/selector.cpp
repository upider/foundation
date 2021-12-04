#include "select/selector.hpp"

Selector::Selector()
{
    _native_handler = ::epoll_create(SELECTOR_MAX_EVENTS);
    assert(_native_handler > 0);
}

Selector::~Selector()
{
    ::close(_native_handler);
}

void Selector::delete_event(epoll_event *ev)
{
    if (ev != NULL)
    {
        if (ev->data.ptr != nullptr)
        {
            auto p = reinterpret_cast<Selectable *>(ev->data.ptr);
            delete p;
        }
        delete ev;
    }
}

Selector::native_handle_type Selector::native_handle() 
{
    return _native_handler;
}

void Selector::add(const Socket &selectable, Selectable::OPCollection ops)
{
    return add(new Socket(selectable), ops);
}

void Selector::add(Socket *selectable, Selectable::OPCollection ops)
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = selectable;
    if (epoll_ctl(_native_handler, EPOLL_CTL_ADD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selector::mod(Socket *selectable, Selectable::OPCollection ops) 
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = selectable;
    if (epoll_ctl(_native_handler, EPOLL_CTL_MOD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selector::remove(const Socket &selectable)
{
    struct epoll_event *ev = NULL;
    if (epoll_ctl(_native_handler, EPOLL_CTL_DEL, selectable.native_handle(), ev) == -1)
    {
        delete_event(ev);
        throw std::runtime_error(strerror(errno));
    }
    delete_event(ev);
}