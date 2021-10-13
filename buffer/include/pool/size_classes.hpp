#ifndef __SIZE_CLASSES_HPP__
#define __SIZE_CLASSES_HPP__

#include "vector"
#include <cinttypes>

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
    static const std::int8_t no = 0;
    static const std::int8_t yes = 1;

    const int _pageSize;
    //用于计算分配内存所在二叉树层数
    const int _pageShifts;
    const int _chunkSize;
    const int _alignment;

    //size表行数
    const int _nSizes;
    //Number of subpages size classes
    int _nSubpages;
    //Number of size classes that are multiples of pageSize
    int _nPSizes;
    //Maximum small size class index
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

public:
    int sizeIdx2size(int sizeIdx);
    int sizeIdx2sizeCompute(int sizeIdx);
    std::int64_t pageIdx2size(int pageIdx);
    std::int64_t pageIdx2sizeCompute(int pageIdx);
    int size2SizeIdx(int size);
    int pages2pageIdx(int pages);
    int pages2pageIdxFloor(int pages);
    //内存对齐数
    int normalizeSize(int size);
    int nSubpages();
    int smallMaxSizeIdx();
    int nSizes();

private:
    // Round size up to the nearest multiple of alignment.
    int alignSize(int size);
    static int normalizeSizeCompute(int size);
    //计算size表行数
    int sizeClasses();
    //计算size表
    int sizeClass(int index, int log2Group, int log2Delta, int nDelta);
    //构造SizeClasses表中index、pageIndex与size的查找表
    void idx2SizeTab(std::vector<int> &sizeIdx2sizeTab, std::vector<int> &pageIdx2sizeTab);
    void size2idxTab(std::vector<int> &size2idxTab);
    int pages2pageIdxCompute(int pages, bool floor);
};

#endif // __SIZE_CLASSES_HPP__