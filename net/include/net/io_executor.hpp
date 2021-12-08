#ifndef __IO_EXECUTOR_HPP__
#define __IO_EXECUTOR_HPP__

#include <memory>

#include "net/select/selectable.hpp"

namespace net
{
    class IOTask;
    class IOExecutor
    {
    public:
        IOExecutor();
        ~IOExecutor();

        void push(std::shared_ptr<IOTask> task);
    };

} // namespace net

#endif /* __IO_EXECUTOR_HPP__ */