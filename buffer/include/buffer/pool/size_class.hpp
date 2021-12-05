#ifndef __SIZE_CLASS_HPP__
#define __SIZE_CLASS_HPP__

#include <vector>

#include "buffer/pool/constant.hpp"

class SizeClass
{
public:
    struct SizeInfo
    {
        PAGE_SIZE_TYPE size_type;
        std::uint64_t cap;
        std::uint64_t free_list_index;
    };
    
public:
    SizeClass();
    ~SizeClass();

    //size转换为index
    std::uint64_t small_size_to_index(std::uint64_t size);
    std::uint64_t normal_size_to_index(std::uint64_t size);
    std::uint64_t huge_size_to_index(std::uint64_t size);
    //index转换为size
    std::uint64_t small_index_to_size(std::uint64_t index);
    std::uint64_t normal_index_to_size(std::uint64_t index);
    std::uint64_t huge_index_to_size(std::uint64_t index);

    PAGE_SIZE_TYPE size_type(std::uint64_t size);
    SizeInfo size_info(std::uint64_t size);
    
};

#endif // __SIZE_CLASS_HPP__