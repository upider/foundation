#include <cmath>

#include "pool/size_classes.hpp"

SizeClasses::SizeClasses(int pageSize, int pageShifts, int chunkSize, int directMemoryCacheAlignment)
    : _pageSize(pageSize), _pageShifts(pageShifts), _chunkSize(chunkSize),
      _directMemoryCacheAlignment(directMemoryCacheAlignment), _nSizes(sizeClasses())
{
    //chunkSize=8192 group=11
    int group = std::log2(chunkSize) + 1 - LOG2_QUANTUM;

    //generate size classes
    //[index, log2Group, log2Delta, nDelta, isMultiPageSize, isSubPage, log2DeltaLookup]
    _sizeClasses = std::vector<std::vector<short>>(group << LOG2_SIZE_CLASS_GROUP, std::vector<short>(7));

    //generate lookup table
    _sizeIdx2sizeTab = std::vector<int>(_nSizes);
    _pageIdx2sizeTab = std::vector<int>(_nSizes);
    idx2SizeTab(_sizeIdx2sizeTab, _pageIdx2sizeTab);

    _size2idxTab = std::vector<int>(_lookupMaxSize >> LOG2_QUANTUM);
    size2idxTab(_size2idxTab);
}

SizeClasses::~SizeClasses()
{
}

int SizeClasses::sizeClasses()
{
    return 0;
}
