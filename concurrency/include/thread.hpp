#ifndef _THREAD_HPP_
#define _THREAD_HPP_

#include <linux/unistd.h>
#include <unistd.h>

#include <cstring>
#include <thread>

/**
 * @brief current_thread_tid 得到线程id
 *
 * @return tid std::thread::id
 */
static std::thread::id current_thread_tid()
{
    return std::this_thread::get_id();
}

/**
 * @brief current_thread_pid 得到线程id
 *
 * @return pid pid_t
 */
static pid_t current_thread_pid()
{
    return syscall(__NR_gettid);
}

/**
 * @brief tid_to_pid std::thread::id转换为pthread_t
 *
 * @param tid std::thread::id
 *
 * @return pthread_t
 */
static pthread_t tid_to_pid(std::thread::id tid)
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

/**
 * @brief thread_name 得到线程名称
 *
 * @param id std::thread::id
 *
 * @return 当前线程名称
 */
static std::string thread_name(std::thread::id id)
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

/**
 * @brief current_thread_name 得到当前线程名称
 *
 * @return std::string ThreadName
 */
static std::string current_thread_name()
{
    return thread_name(std::this_thread::get_id());
}

/**
 * @brief tid_thread_name 设置线程名称
 *
 * @param tid std::thread::id
 * @param name 线程名称
 *
 * @return true - 成功
 */
static bool tid_thread_name(std::thread::id tid, const std::string &name)
{
    auto str = name.substr(0, 15);
    char buf[16] = {};
    std::memcpy(buf, str.data(), str.size());
    auto id = tid_to_pid(tid);
    return 0 == pthread_setname_np(id, buf);
}

/**
 * @brief pid_thread_name 设置线程名称
 *
 * @param tid std::thread::id
 * @param name 线程名称
 *
 * @return true - 成功
 */
static bool pid_thread_name(pthread_t pid, const std::string &name)
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

/**
 * @brief current_thread_name 设置当前线程名称
 *
 * @param name 线程名称
 *
 * @return true - 成功
 */
static bool current_thread_name(const std::string &name)
{
    return tid_thread_name(std::this_thread::get_id(), name);
}

#include <atomic>
#include <functional>

#include "runnable.hpp"

using Func = std::function<void()>;

class Thread
{
private:
    class ThreadRunnable : virtual public Runnable
    {
    private:
        std::function<void()> _fun;

    public:
        ThreadRunnable(std::function<void()> f);
        virtual void run() override;
    };

private:
    std::thread _thread;
    std::string _name{""};
    pid_t _pid = -1;
    std::thread::id _tid;
    std::atomic_bool _running{false};

    std::unique_ptr<Runnable> _target;

public:
    Thread() = default;
    ~Thread();

    /**
     * @brief Thread 构造函数
     *
     * @param f 要执行的任务
     * @param name 线程名
     */
    Thread(std::unique_ptr<Runnable> f, const std::string &name = "");

    /**
     * @brief Thread 构造函数
     *
     * @param f 要执行的任务
     * @param name 线程名
     */
    Thread(std::function<void()> f, const std::string &name = "");

    /**
     * @brief 获取线程pid
     *
     * @return pid_t
     */
    pid_t get_pid();
    /**
     * @brief 获取线程tid
     *
     * @return std::thread::id
     */
    std::thread::id get_tid();
    std::string get_name();
    void set_name(const std::string &name);
    void start();
    void join();
    void detach();
    bool joinable();
    void run();
    bool is_running();
    static void sleep(long seconds);
    static void sleep(long milis, long nanos);

private:
    void cleanup();
    void init();
};

#endif // _THREAD_HPP_