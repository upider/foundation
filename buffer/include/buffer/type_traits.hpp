#ifndef __TYPE_TRAITS_HPP__
#define __TYPE_TRAITS_HPP__

#include "byte/byte.hpp"

template <typename T, typename Enable = void>
struct is_readable
{
};

template <typename T>
struct is_readable<T, typename std::enable_if<
                          std::is_same<T, u_char>::value ||
                          std::is_same<T, char>::value ||
                          std::is_same<T, Byte>::value ||
                          std::is_arithmetic<T>::value>::type>
{
    typedef T type;
};

#endif /* __TYPE_TRAITS_HPP__ */
