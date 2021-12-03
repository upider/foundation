#include "select/selector.hpp"
#include "select/selected.hpp"

Selected::Selected(Selector *selector, Selectable *selectable, Selectable::OperationCollection op)
    :  _selector(selector), _selectable(selectable),_op(op) {}

Selected::~Selected() {}

Selector& Selected::selector() 
{
    return *_selector;    
}

Selectable& Selected::selectable()
{
    return *_selectable;
}
Selectable::OperationCollection Selected::operation()
{
    return _op;
}

void Selected::release()
{
    _selectable->close();
    delete _selectable;
    _selectable = nullptr;
}

void Selected::select(Selectable::OperationCollection ops) 
{
    _selector->mod(_selectable, ops);
}