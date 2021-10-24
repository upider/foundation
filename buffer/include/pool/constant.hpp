#ifndef __CONSTANT_HPP__
#define __CONSTANT_HPP__

constexpr const unsigned long long KB = 1024;
constexpr const unsigned long long MB = 1048576;
constexpr const unsigned long long SMALLS_MIN_IDX = 6;
constexpr const unsigned long long SMALLS_LEN = 12;
constexpr const unsigned long long NORMALS_LEN = 4096 + 1;
constexpr const unsigned long long HUGES_LEN = 48;

enum class PAGE_SIZE_TYPE
{
    SMALL,
    NORMAL,
    BIG,
    UNMANAGE
};

#endif // __CONSTANT_HPP__