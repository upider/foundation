#include "concurrency/concurrent_queue/concurrent_queue.hpp"
#include "concurrency/thread/thread.hpp"
#include "concurrency/executor/executor_task.hpp"
#include "concurrency/executor/executor.hpp"
#include <iostream>

Executor::Thread::Thread(std::thread thread) : _thread(std::move(thread)) {}
Executor::Thread::~Thread()
{
    _thread.join();
}

Executor::Executor(std::size_t threads, std::unique_ptr<ConcurrentQueue<std::shared_ptr<ExecutorTask>>> task_queue, std::shared_ptr<ThreadFactory> thread_factory)
    : _thread_num(threads), _task_queue(std::move(task_queue)), _thread_factory(thread_factory), _threads(_thread_num)
{
    init_threads();
}

Executor::~Executor() {}

void Executor::init_threads()
{
    for (std::size_t i = 0; i < _thread_num; i++)
    {
        _threads.emplace_back(new Thread(_thread_factory->get(std::bind(&Executor::task, this))));
    }
}

void Executor::task()
{
    while (1)
    {
        switch (_phase)
        {
        case RUNNING:
        {
            ExecutorTaskSptr task;
            if (_task_queue->wait_pop(task, 0, 1000))
            {
                task->run();
            }
            break;
        }
        case SHUTDOWON:
        {
            std::shared_ptr<ExecutorTask> task;
            if (_task_queue->try_pop(task))
            {
                task->run();
            }
            else
            {
                return;
            }
            break;
        }
        case STOP:
        {
            return;
        }
        default:
            break;
        }
    }
}

void Executor::shutdown()
{
    _phase.store(SHUTDOWON);
}

void Executor::stop()
{
    _phase.store(STOP);
}

std::size_t Executor::threads()
{
    return _thread_num;
}

void Executor::thread_factory(std::shared_ptr<ThreadFactory> factory)
{
    _thread_factory = factory;
}

std::shared_ptr<ThreadFactory> Executor::thread_factory()
{
    return _thread_factory;
}