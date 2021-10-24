#include <cinttypes>
#include <cmath>

#include "pool/size_class.hpp"

SizeClass::SizeClass()
{
}

SizeClass::~SizeClass()
{
}

PAGE_SIZE_TYPE SizeClass::size_type(std::uint64_t size)
{
    // size <= 2048, SMALL
    // 2048 < size <= 16M, NORMAL
    // 16M < size <= 64M, BIG
    if (size <= 2048)
    {
        return PAGE_SIZE_TYPE::SMALL;
    }
    else if (size <= 16 * MB)
    {
        return PAGE_SIZE_TYPE::NORMAL;
    }
    else if (size <= 64 * MB)
    {
        return PAGE_SIZE_TYPE::BIG;
    }
    else
    {
        return PAGE_SIZE_TYPE::UNMANAGE;
    }
}

std::uint64_t SizeClass::small_size_to_index(std::uint64_t size)
{
    // 2**6=64, 最小分配16字节
    std::uint64_t idx = ceil(std::log2(size));
    return idx < SMALLS_MIN_IDX ? SMALLS_MIN_IDX : idx;
}

std::uint64_t SizeClass::normal_size_to_index(std::uint64_t size)
{
    return (size + 1) / (4 * KB) + 1;
}

std::uint64_t SizeClass::huge_size_to_index(std::uint64_t size)
{
    return (size - 16 * MB) / MB;
}

std::uint64_t SizeClass::small_index_to_size(std::uint64_t index)
{
    return 1ULL << index;
}

std::uint64_t SizeClass::normal_index_to_size(std::uint64_t index)
{
    return 4 * KB * index;
}

std::uint64_t SizeClass::huge_index_to_size(std::uint64_t index)
{
    return 16 * MB * index + 1 * MB;
}