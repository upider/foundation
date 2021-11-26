#ifndef __DEFER_HPP__
#define __DEFER_HPP__

#include <functional>
#include <future>
#include <iostream>

#define defer_name_cat(x, c) x##f##l##c
#define defer_make_name(x, c) defer_name_cat(x, c)
#define defer_name defer_make_name(_defer_, __COUNTER__)
#define defer(...) auto defer_name = create_defer(__VA_ARGS__)

template <typename Fn>
class ScopeGuard
{
private:
    Fn _func;

public:
    ScopeGuard() = delete;
    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard &operator=(const ScopeGuard &) = delete;

    ScopeGuard(ScopeGuard && other) : _func(std::move(other._func)) {}
    ScopeGuard &operator=(ScopeGuard && other) { _func = std::move(other._func); }
    ScopeGuard(Fn &&f) : _func(std::forward<Fn>(f)) {}
    ~ScopeGuard() { _func(); }
};

template <typename Callable, typename... Args>
inline ScopeGuard<std::thread::_Invoker<std::tuple<typename std::decay<Callable>::type, typename std::decay<Args>::type...>>> create_defer(Callable &&f, Args &&...args)
{
    using TUPLE = std::tuple<typename std::decay<Callable>::type, typename std::decay<Args>::type...>;
    static_assert(std::__is_invocable<typename std::decay<Callable>::type, typename std::decay<Args>::type...>::value, "defer arguments must be invocable after conversion to rvalues");
    std::thread::_Invoker<TUPLE> invoke = std::thread::__make_invoker(std::forward<Callable>(f), std::forward<Args>(args)...);
    return ScopeGuard<std::thread::_Invoker<TUPLE>>(std::move(invoke));
}

#endif /* __DEFER_HPP__ */
