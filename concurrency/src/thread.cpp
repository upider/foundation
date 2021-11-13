#include <cstring>

#include "concurrency/thread/thread.hpp"

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