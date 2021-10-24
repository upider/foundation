#ifndef __STACK_TRACE_HPP__
#define __STACK_TRACE_HPP__

#include <string>
#include <vector>

/**
 * 得到string类型堆栈信息
*/
std::string stacktrace_str(std::size_t stackSize, std::size_t skip = 0);

/**
 * 得到vector类型堆栈信息
*/
std::vector<std::string> stacktrace_vec(std::size_t stackSize, std::size_t skip = 0);

#endif // __STACK_TRACE_HPP__