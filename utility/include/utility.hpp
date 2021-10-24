#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

#include <cinttypes>
#include <type_traits>

//逻辑右移
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
logic_rshift(T num, int shifts)
{
    typename std::make_unsigned<T>::type tmp = num;
    return  tmp >> shifts;
}

//最小公倍数
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T x, T y)
{
    return y ? gcd(y, x % y) : x;
}

#endif // __UTILITY_HPP__