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

void Selector::add(const Selectable &selectable, Selectable::OperationCollection ops)
{
    return add(new Selectable(selectable), ops);
}

void Selector::add(Selectable *selectable, Selectable::OperationCollection ops)
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = selectable;
    if (epoll_ctl(_native_handler, EPOLL_CTL_ADD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selector::mod(Selectable *selectable, Selectable::OperationCollection ops) 
{
    struct epoll_event ev;
    ev.events = ops;
    ev.data.ptr = selectable;
    if (epoll_ctl(_native_handler, EPOLL_CTL_MOD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selector::remove(const Selectable &selectable)
{
    struct epoll_event *ev = NULL;
    if (epoll_ctl(_native_handler, EPOLL_CTL_DEL, selectable.native_handle(), ev) == -1)
    {
        delete_event(ev);
        throw std::runtime_error(strerror(errno));
    }
    delete_event(ev);
}