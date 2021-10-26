#include <stdexcept>
#include <chrono>

#include "thread/thread.hpp"

std::thread::id current_thread_tid()
{
    return std::this_thread::get_id();
}

pid_t current_thread_pid()
{
    return syscall(__NR_gettid);
}

pthread_t tid_to_pid(std::thread::id tid)
{
    static_assert(
        std::is_same<pthread_t, std::thread::native_handle_type>::value,
        "This assumes that the native handle type is pthread_t");
    static_assert(
        sizeof(std::thread::native_handle_type) == sizeof(std::thread::id),
        "This assumes std::thread::id is a thin wrapper around "
        "std::thread::native_handle_type, but that doesn't appear to be true.");
    // In most implementations, std::thread::id is a thin wrapper around
    // std::thread::native_handle_type, which means we can do unsafe things to
    // extract it.
    pthread_t id;
    std::memcpy(&id, &tid, sizeof(id));
    return id;
}

std::string thread_name(std::thread::id id)
{
    std::array<char, 16> buf;
    if (id != std::thread::id() &&
        pthread_getname_np(tid_to_pid(id), buf.data(), buf.size()) == 0)
    {
        return std::string(buf.data());
    }
    else
    {
        return "";
    }
}

std::string current_thread_name()
{
    return thread_name(std::this_thread::get_id());
}

bool tid_thread_name(std::thread::id tid, const std::string &name)
{
    auto str = name.substr(0, 15);
    char buf[16] = {};
    std::memcpy(buf, str.data(), str.size());
    auto id = tid_to_pid(tid);
    return 0 == pthread_setname_np(id, buf);
}

bool pid_thread_name(pthread_t pid, const std::string &name)
{
    static_assert(
        std::is_same<pthread_t, std::thread::native_handle_type>::value,
        "This assumes that the native handle type is pthread_t");
    static_assert(
        sizeof(std::thread::native_handle_type) == sizeof(std::thread::id),
        "This assumes std::thread::id is a thin wrapper around "
        "std::thread::native_handle_type, but that doesn't appear to be true.");
    // In most implementations, std::thread::id is a thin wrapper around
    // std::thread::native_handle_type, which means we can do unsafe things to
    // extract it.
    std::thread::id id;
    std::memcpy(static_cast<void *>(&id), &pid, sizeof(id));
    return tid_thread_name(id, name);
}

bool current_thread_name(const std::string &name)
{
    return tid_thread_name(std::this_thread::get_id(), name);
}

Thread::ThreadRunnable::ThreadRunnable(std::function<void()> f)
    : _fun(std::move(f)) {}

void Thread::ThreadRunnable::run()
{
    _fun();
}

Thread::Thread(std::shared_ptr<Runnable> f, const std::string &name)
    : _name(name), _target(f) {}

Thread::Thread(std::function<void()> f, const std::string &name)
    : _name(name), _target(new ThreadRunnable(std::move(f))) {}

Thread::~Thread()
{
    if (joinable())
    {
        join();
    }
}

pid_t Thread::get_pid()
{
    return _pid;
}

std::string Thread::get_name()
{
    return _name;
}

void Thread::set_name(const std::string &name)
{
    _name = name;
    tid_thread_name(_thread.get_id(), name);
}

void Thread::join()
{
    _thread.join();
}

void Thread::detach()
{
    _thread.detach();
}

bool Thread::joinable()
{
    return _thread.joinable();
}

void Thread::start()
{
    if (_running.load(std::memory_order_relaxed))
    {
        throw std::runtime_error("thrad alread running");
    }
    else
    {
        _running.store(true);
        _thread = std::thread([this]()
                              { run(); });
    }
}

void Thread::run()
{
    try
    {
        if (_target != nullptr)
        {
            init();
            _target->run();
            cleanup();
        }
    }
    catch (...)
    {
        cleanup();
        throw;
    }
}

bool Thread::is_running()
{
    return _running.load();
}

void Thread::sleep(long seconds)
{
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

void Thread::sleep(long milis, long nanos)
{
    nanos = nanos + milis * 10e6;
    std::this_thread::sleep_for(std::chrono::nanoseconds(nanos));
}

void Thread::init()
{
    if (_name != "")
    {
        current_thread_name(_name);
    }
    else
    {
        _name = current_thread_name();
    }
    _running.store(true);
    _pid = syscall(__NR_gettid);
    _tid = _thread.get_id();
}

void Thread::cleanup()
{
    _running.store(false);
    _pid = -1;
}