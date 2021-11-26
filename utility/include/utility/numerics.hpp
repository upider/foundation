#ifndef __NUMERICS_HPP__
#define __NUMERICS_HPP__

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

#endif /* __NUMERICS_HPP__ */
