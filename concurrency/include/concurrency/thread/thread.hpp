#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <linux/unistd.h>
#include <unistd.h>

#include <string>
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

#endif // __THREAD_HPP__