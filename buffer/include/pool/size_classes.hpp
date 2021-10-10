#ifndef __SIZE_CLASSES_H__
#define __SIZE_CLASSES_H__

#include "vector"

#include "byte/byte.hpp"

class SizeClasses
{
public:
    static const int LOG2_QUANTUM = 4;

private:
    static const int LOG2_SIZE_CLASS_GROUP = 2;
    static const int LOG2_MAX_LOOKUP_SIZE = 12;
    static const int INDEX_IDX = 0;
    static const int LOG2GROUP_IDX = 1;
    static const int LOG2DELTA_IDX = 2;
    static const int NDELTA_IDX = 3;
    static const int PAGESIZE_IDX = 4;
    static const int SUBPAGE_IDX = 5;
    static const int LOG2_DELTA_LOOKUP_IDX = 6;
    static const Byte no = to_byte(0);
    static const Byte yes = to_byte(1);

    const int _pageSize;
    const int _pageShifts;
    const int _chunkSize;
    const int _directMemoryCacheAlignment;

    const int _nSizes;
    int _nSubpages;
    int _nPSizes;
    int _smallMaxSizeIdx;
    //在sizeClasses()函数中初始化
    int _lookupMaxSize;

    std::vector<std::vector<short>> _sizeClasses;
    std::vector<int> _pageIdx2sizeTab;
    // // lookup table for sizeIdx <= smallMaxSizeIdx
    std::vector<int> _sizeIdx2sizeTab;
    // lookup table used for size <= lookupMaxclass
    // spacing is 1 << LOG2_QUANTUM, so the size of array is lookupMaxclass >> LOG2_QUANTUM
    std::vector<int> _size2idxTab;

public:
    SizeClasses(int pageSize, int pageShifts, int chunkSize, int directMemoryCacheAlignment = 1);
    ~SizeClasses();

private:
    int sizeClasses();
    void idx2SizeTab(std::vector<int>&, std::vector<int>&);
    void size2idxTab(std::vector<int>&);
};

#endif // __SIZE_CLASSES_H__