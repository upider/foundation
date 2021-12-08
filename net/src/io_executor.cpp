#include "net/io_task.hpp"
#include "net/io_executor.hpp"

namespace net
{
    IOExecutor::IOExecutor()
    {
    }

    IOExecutor::~IOExecutor()
    {
    }
    
    void IOExecutor::push(std::shared_ptr<IOTask> task)
    {
    }

} // namespace net
