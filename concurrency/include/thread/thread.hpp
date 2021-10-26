#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <linux/unistd.h>
#include <unistd.h>

#include <cstring>
#include <thread>

/**
 * @brief current_thread_tid 得到线程id
 *
 * @return tid std::thread::id
 */
std::thread::id current_thread_tid();

/**
 * @brief current_thread_pid 得到线程id
 *
 * @return pid pid_t
 */
pid_t current_thread_pid();

/**
 * @brief tid_to_pid std::thread::id转换为pthread_t
 *
 * @param tid std::thread::id
 *
 * @return pthread_t
 */
pthread_t tid_to_pid(std::thread::id tid);

/**
 * @brief thread_name 得到线程名称
 *
 * @param id std::thread::id
 *
 * @return 当前线程名称
 */
std::string thread_name(std::thread::id id);

/**
 * @brief current_thread_name 得到当前线程名称
 *
 * @return std::string ThreadName
 */
std::string current_thread_name();

/**
 * @brief tid_thread_name 设置线程名称
 *
 * @param tid std::thread::id
 * @param name 线程名称
 *
 * @return true - 成功
 */
bool tid_thread_name(std::thread::id tid, const std::string &name);

/**
 * @brief pid_thread_name 设置线程名称
 *
 * @param tid std::thread::id
 * @param name 线程名称
 *
 * @return true - 成功
 */
bool pid_thread_name(pthread_t pid, const std::string &name);

/**
 * @brief current_thread_name 设置当前线程名称
 *
 * @param name 线程名称
 *
 * @return true - 成功
 */
bool current_thread_name(const std::string &name);

#include <atomic>
#include <functional>

#include "thread/runnable.hpp"

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

    std::shared_ptr<Runnable> _target;

public:
    Thread() = default;
    ~Thread();

    Thread &operator=(const Thread &) = delete;
    Thread(const Thread &) = delete;

public:
    /**
     * @brief Thread 构造函数
     *
     * @param f 要执行的任务
     * @param name 线程名
     */
    Thread(std::shared_ptr<Runnable> f, const std::string &name = "");

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

#endif // __THREAD_HPP__