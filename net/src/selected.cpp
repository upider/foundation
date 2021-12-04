#include "select/selector.hpp"
#include "select/selected.hpp"
#include "socket/socket.hpp"

Selected::Selected(Selector *selector, Socket *selectable, Selectable::OPCollection op)
    :  _selector(selector), _selectable(selectable), _op(op) {}

Selected::~Selected() {}

Selector& Selected::selector() 
{
    return *_selector;    
}

Socket& Selected::selectable()
{
    return *_selectable;
}

Selectable::OPCollection Selected::operation()
{
    return _op;
}

void Selected::release()
{
    _selector->remove(*_selectable);
    _selectable->close();
    delete _selectable;
    _selectable = nullptr;
}

void Selected::select(Selectable::OPCollection ops) 
{
    _selector->mod(_selectable, ops);
}