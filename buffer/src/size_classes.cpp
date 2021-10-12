#include <cassert>
#include <cmath>

#include "pool/size_classes.hpp"

SizeClasses::SizeClasses(int pageSize, int pageShifts, int chunkSize, int directMemoryCacheAlignment)
    : _pageSize(pageSize), _pageShifts(pageShifts), _chunkSize(chunkSize),
      _directMemoryCacheAlignment(directMemoryCacheAlignment), _nSizes(sizeClasses())
{
    //chunkSize=8192 group=11？？？？
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
    int normalMaxSize = -1;
    int index = 0;
    int size = 0;
    int log2Group = LOG2_QUANTUM;
    int log2Delta = LOG2_QUANTUM;
    int ndeltaLimit = 1 << LOG2_SIZE_CLASS_GROUP;

    //First small group, nDelta start at 0.
    //first size class is 1 << LOG2_QUANTUM
    int nDelta = 0;
    while (nDelta < ndeltaLimit)
    {
        size = sizeClass(index++, log2Group, log2Delta, nDelta++);
    }
    log2Group += LOG2_SIZE_CLASS_GROUP;

    //All remaining groups, nDelta start at 1.
    while (size < _chunkSize)
    {
        nDelta = 1;

        while (nDelta <= ndeltaLimit && size < _chunkSize)
        {
            size = sizeClass(index++, log2Group, log2Delta, nDelta++);
            normalMaxSize = size;
        }

        log2Group++;
        log2Delta++;
    }

    //chunkSize must be normalMaxSize
    assert(_chunkSize == normalMaxSize);

    //return number of size index
    return index;
}

int SizeClasses::sizeClass(int index, int log2Group, int log2Delta, int nDelta)
{
    short isMultiPageSize;
    if (log2Delta >= _pageShifts)
    {
        isMultiPageSize = yes;
    }
    else
    {
        int pageSize = 1 << _pageShifts;
        int size = (1 << log2Group) + (1 << log2Delta) * nDelta;

        isMultiPageSize = size == size / pageSize * pageSize ? yes : no;
    }

    int log2Ndelta = nDelta == 0 ? 0 : log2(nDelta);

    auto remove = 1 << log2Ndelta < nDelta ? yes : no;

    int log2Size = log2Delta + log2Ndelta == log2Group ? log2Group + 1 : log2Group;
    if (log2Size == log2Group)
    {
        remove = yes;
    }

    short isSubpage = log2Size < _pageShifts + LOG2_SIZE_CLASS_GROUP ? yes : no;

    int log2DeltaLookup = log2Size < LOG2_MAX_LOOKUP_SIZE || (log2Size == LOG2_MAX_LOOKUP_SIZE && remove == no) ? log2Delta : no;

    std::vector<short> sz({(short)index, (short)log2Group, (short)log2Delta,
                           (short)nDelta, isMultiPageSize, isSubpage, (short)log2DeltaLookup});

    _sizeClasses[index] = sz;
    int size = (1 << log2Group) + (nDelta << log2Delta);

    if (sz[PAGESIZE_IDX] == yes)
    {
        _nPSizes++;
    }
    if (sz[SUBPAGE_IDX] == yes)
    {
        _nSubpages++;
        _smallMaxSizeIdx = index;
    }
    if (sz[LOG2_DELTA_LOOKUP_IDX] != no)
    {
        _lookupMaxSize = size;
    }
    return size;
}

void SizeClasses::idx2SizeTab(std::vector<int> &sizeIdx2sizeTab, std::vector<int> &pageIdx2sizeTab)
{
    int pageIdx = 0;

    for (int i = 0; i < _nSizes; i++)
    {
        auto sizeClass = _sizeClasses[i];
        int log2Group = sizeClass[LOG2GROUP_IDX];
        int log2Delta = sizeClass[LOG2DELTA_IDX];
        int nDelta = sizeClass[NDELTA_IDX];

        int size = (1 << log2Group) + (nDelta << log2Delta);
        sizeIdx2sizeTab[i] = size;

        if (sizeClass[PAGESIZE_IDX] == yes)
        {
            pageIdx2sizeTab[pageIdx++] = size;
        }
    }
}

void SizeClasses::size2idxTab(std::vector<int> &size2idxTab)
{
    int idx = 0;
    int size = 0;

    for (int i = 0; size <= _lookupMaxSize; i++)
    {
        int log2Delta = _sizeClasses[i][LOG2DELTA_IDX];
        int times = (1 << log2Delta) - LOG2_QUANTUM;

        while (size <= _lookupMaxSize && times-- > 0)
        {
            size2idxTab[idx++] = i;
            size = idx + (1 << LOG2_QUANTUM);
        }
    }
}

int SizeClasses::alignSize(int size)
{
    int delta = size & (_directMemoryCacheAlignment - 1);
    return delta == 0 ? size : size + _directMemoryCacheAlignment - delta;
}

int SizeClasses::normalizeSizeCompute(int size)
{
    int x = log2((size << 1) - 1);
    int log2Delta = x < LOG2_SIZE_CLASS_GROUP + LOG2_QUANTUM + 1
                        ? LOG2_QUANTUM
                        : x - LOG2_SIZE_CLASS_GROUP - 1;
    int delta = 1 << log2Delta;
    int delta_mask = delta - 1;
    return (size + delta_mask) & ~delta_mask;
}

int SizeClasses::normalizeSize(int size)
{
    if (size == 0)
    {
        return _sizeIdx2sizeTab[0];
    }
    if (_directMemoryCacheAlignment > 0)
    {
        size = alignSize(size);
    }

    if (size <= _lookupMaxSize)
    {
        int ret = _sizeIdx2sizeTab[_size2idxTab[(size - 1) >> LOG2_QUANTUM]];
        assert(ret == normalizeSizeCompute(size));
        return ret;
    }
    return normalizeSizeCompute(size);
}

std::int64_t SizeClasses::pageIdx2size(int pageIdx)
{
    return _pageIdx2sizeTab[pageIdx];
}

std::int64_t SizeClasses::pageIdx2sizeCompute(int pageIdx)
{
    int group = pageIdx >> LOG2_SIZE_CLASS_GROUP;
    int mod = pageIdx & ((1 << LOG2_SIZE_CLASS_GROUP) - 1);

    long groupSize = group == 0 ? 0 : 1L << (_pageShifts + (LOG2_SIZE_CLASS_GROUP - 1)) << group;

    int shift = group == 0 ? 1 : group;
    int log2Delta = shift + _pageShifts - 1;
    int modSize = (mod + 1) << log2Delta;

    return groupSize + modSize;
}

int SizeClasses::size2SizeIdx(int size)
{
    if (size == 0)
    {
        return 0;
    }
    if (size > _chunkSize)
    {
        return _nSizes;
    }

    if (_directMemoryCacheAlignment > 0)
    {
        size = alignSize(size);
    }

    if (size <= _lookupMaxSize)
    {
        //size-1 / MIN_TINY
        return _size2idxTab[(size - 1) >> LOG2_QUANTUM];
    }

    int x = log2((size << 1) - 1);
    int shift = x < LOG2_SIZE_CLASS_GROUP + LOG2_QUANTUM + 1
                    ? 0
                    : x - (LOG2_SIZE_CLASS_GROUP + LOG2_QUANTUM);

    int group = shift << LOG2_SIZE_CLASS_GROUP;

    int log2Delta = x < LOG2_SIZE_CLASS_GROUP + LOG2_QUANTUM + 1
                        ? LOG2_QUANTUM
                        : x - LOG2_SIZE_CLASS_GROUP - 1;

    int deltaInverseMask = -1 << log2Delta;
    int mod = ((size - 1) & deltaInverseMask) >> log2Delta &
              ((1 << LOG2_SIZE_CLASS_GROUP) - 1);

    return group + mod;
}

int SizeClasses::pages2pageIdx(int pages) { return pages2pageIdxCompute(pages, false); }

int SizeClasses::pages2pageIdxFloor(int pages) { return pages2pageIdxCompute(pages, true); }

int SizeClasses::pages2pageIdxCompute(int pages, bool floor)
{
    int pageSize = pages << _pageShifts;
    if (pageSize > _chunkSize)
    {
        return _nPSizes;
    }

    int x = log2((pageSize << 1) - 1);

    int shift = x < LOG2_SIZE_CLASS_GROUP + _pageShifts
                    ? 0
                    : x - (LOG2_SIZE_CLASS_GROUP + _pageShifts);

    int group = shift << LOG2_SIZE_CLASS_GROUP;

    int log2Delta = x < LOG2_SIZE_CLASS_GROUP + _pageShifts + 1 ? _pageShifts : x - LOG2_SIZE_CLASS_GROUP - 1;

    int deltaInverseMask = -1 << log2Delta;
    int mod = ((pageSize - 1) & deltaInverseMask) >> log2Delta &
              ((1 << LOG2_SIZE_CLASS_GROUP) - 1);

    int pageIdx = group + mod;

    if (floor && _pageIdx2sizeTab[pageIdx] > pages << _pageShifts)
    {
        pageIdx--;
    }

    return pageIdx;
}