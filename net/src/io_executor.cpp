#include "net/io_task.hpp"
#include "net/io_executor.hpp"

namespace net
{
    IOExecutor::IOExecutor(std::size_t threads) :  _thread_num(threads), _selectors(threads)
    {
        for (size_t i = 0; i < _thread_num; i++)
        {
            _threads.push_back(std::thread(std::bind(&IOExecutor::worker, this, i)));
        }
    }

    IOExecutor::~IOExecutor()
    {
        for (auto& thread : _threads)
        {
            thread.join();
        }
    }

    void IOExecutor::stop() 
    {
        _running = false;
    }
    
    void IOExecutor::push(std::shared_ptr<IOTask> task)
    {
        auto index = task->native_handle() % _thread_num;
        _selectors[index].add(task->native_handle(), task->interest(), task);
    }

    void IOExecutor::operator()(std::size_t n)
    {
        auto &selector = _selectors[n];
        std::vector<net::select::Selected<std::shared_ptr<IOTask>>> vec;
        while (_running)
        {
            std::size_t size = selector.select(vec, std::chrono::milliseconds(500));
            for (size_t i = 0; i < size; i++)
            {
                auto& selected = vec[i];
                auto ops = selected.operation();
                std::shared_ptr<IOTask>& taks = selected.attachment();
                taks->operator()(ops);
                if(ops & net::Selectable::OP::EXCEPT)
                {
                    selected.release();
                }
                else
                {
                    selected.select(taks->interest());
                }
            }
        }
    }

} // namespace net
