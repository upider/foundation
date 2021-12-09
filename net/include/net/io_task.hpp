#ifndef __IO_TASK_HPP__
#define __IO_TASK_HPP__

#include <functional>

#include "net/select/selectable.hpp"

namespace net
{
    using namespace select;
    class IOTask
    {
    public:
        IOTask() = default;
        virtual ~IOTask() = default;
        virtual void operator()(Selectable::OPCollection ops) = 0;
        virtual Selectable::OPCollection interest() = 0;
        virtual Selectable::native_handle_type native_handle() = 0;
    };

} // namespace net

#endif /* __IO_TASK_HPP__ */
