#ifndef __IO_EXECUTOR_HPP__
#define __IO_EXECUTOR_HPP__

#include <memory>
#include <thread>
#include <list>
#include <vector>
#include <atomic>

#include "net/select/selector.hpp"

namespace net
{
    class IOTask;
    class IOExecutor
    {
    private:
        std::atomic_bool _running{true};
        std::size_t _thread_num;
        std::vector<select::Selector<std::shared_ptr<IOTask>>> _selectors;
        std::list<std::thread> _threads;

        void operator()(std::size_t n);

    public:
        IOExecutor(std::size_t threads = 1);
        ~IOExecutor();

        void stop();

        void push(std::shared_ptr<IOTask> task);
    };

} // namespace net

#endif /* __IO_EXECUTOR_HPP__ */
