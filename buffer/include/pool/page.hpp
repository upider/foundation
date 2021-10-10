#ifndef __PAGE_HPP__
#define __PAGE_HPP__

#include "byte/byte.hpp"

//8kB page
typedef struct
{
    Byte p[2 << 13];
} page_t;

#endif // __PAGE_HPP__