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

uint32_t Selector::op_to_num(uint32_t ops)
{
    uint32_t read = 0, write = 0, close = 0, oneshot = 0, except = 0;

    except = ops & Selectable::Operation::OP_EXCEPT;
    close = ops & Selectable::Operation::OP_REMOTECLOSE;
    write = ops & Selectable::Operation::OP_WRITE;
    read = ops & Selectable::Operation::OP_READ;
    oneshot = ops & Selectable::Operation::OP_ONESHOT;
    return read | write | oneshot | close | except;
}

Selectable::OperationCollection Selector::num_to_op(Selectable &st, Selectable::OperationCollection what)
{
    //num 可能是Selectable::Operation::OP_READ
    //Selectable::Operation::OP_WRITE
    //Selectable::Operation::OP_REMOTECLOSE
    //Selectable::Operation::OP_ONESHOT
    //Selectable::Operation::OP_EXCEPT
    uint32_t read = 0, write = 0, oneshot = 0, close = 0, except = 0;
    except = what & Selectable::Operation::OP_EXCEPT ? Selectable::Operation::OP_EXCEPT : 0;
    close = what & Selectable::Operation::OP_REMOTECLOSE ? Selectable::Operation::OP_REMOTECLOSE : 0;
    write = what & Selectable::Operation::OP_WRITE ? Selectable::Operation::OP_WRITE : 0;
    read = what & Selectable::Operation::OP_READ ? Selectable::Operation::OP_READ : 0;
    oneshot = what & Selectable::Operation::OP_ONESHOT ? Selectable::Operation::OP_ONESHOT : 0;
    return read | write | oneshot | close | except;
}

void Selector::add(const Selectable &selectable, Selectable::OperationCollection ops)
{
    return add(new Selectable(selectable), ops);
}

void Selector::add(Selectable *selectable, Selectable::OperationCollection ops)
{
    struct epoll_event ev;
    ev.events = op_to_num(ops);
    ev.data.ptr = selectable;
    if (epoll_ctl(_native_handler, EPOLL_CTL_ADD, selectable->native_handle(), &ev) == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selector::mod(Selectable *selectable, Selectable::OperationCollection ops) 
{
    struct epoll_event ev;
    ev.events = op_to_num(ops);
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