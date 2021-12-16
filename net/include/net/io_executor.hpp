#ifndef __IO_EXECUTOR_HPP__
#define __IO_EXECUTOR_HPP__

#include <memory>
#include <thread>
#include <list>
#include <vector>
#include <atomic>
#include <unordered_map>

#include "net/select/selector.hpp"

namespace net
{
    class IOTask;
    // TODO
    class IOExecutor
    {
    private:
        struct SelectorThread
        {
        private:
            select::Selector<int> selector;
            std::thread thread;
            std::unordered_map<select::Selectable::native_handle_type, std::pair<select::Selectable::OPCollection, std::list<std::shared_ptr<IOTask>>>> tasks;
        public:
            void add(std::shared_ptr<IOTask>);
        };

    private:
        std::atomic_bool _running{true};
        std::size_t _thread_num;
        std::list<SelectorThread> _threads;

        void worker(std::size_t n);

    public:
        IOExecutor(std::size_t threads = 1);
        ~IOExecutor();

        void stop();

        void push(std::shared_ptr<IOTask> task);
    };

} // namespace net

#endif /* __IO_EXECUTOR_HPP__ */
