#include "pool/page.hpp"
#include "pool/chunk.hpp"
#include "pool/arena.hpp"
#include "pool/priority_queue.hpp"

Chunk::Chunk(Arena *arena, int pageSize, int pageShifts, int chunkSize, int maxPageIdx)
    : _arena(arena), _pageSize(pageSize), _pageShifts(pageShifts), _chunkSize(chunkSize),
      _pages(new Page[pageSize]), _runsAvailMap(),
      _runsAvail(maxPageIdx, HandlePriorityQueue()), _subpages(chunkSize >> pageShifts)
{
    //insert initial run, offset = 0, pages = chunkSize / pageSize
    int pages = _chunkSize >> _pageShifts;
    //page 数量从低34位开始
    handle_t initHandle = (long)pages << SIZE_SHIFT;
    insertAvailRun(0, pages, initHandle);
}

Chunk::~Chunk()
{
    if (_pages != nullptr)
    {
        delete _pages;
    }
}

void Chunk::insertAvailRun(offset_t runOffset, int pages, handle_t handle)
{
    auto pageIdxFloor = pages2pageIdxFloor(pages);
    _runsAvail[pageIdxFloor].push(handle);
    //insert first page of run
    insertAvailRun0(runOffset, handle);
    if (pages > 1)
    {
        //insert last page of run
        insertAvailRun0(lastPage(runOffset, pages), handle);
    }
}

void Chunk::insertAvailRun0(offset_t runOffset, handle_t handle)
{
    _runsAvailMap.emplace(runOffset, handle);
}

void Chunk::removeAvailRun(handle_t handle)
{
    //pageIdx表示_runsAvail中的run中的page index
    int pageIdxFloor = pages2pageIdxFloor(runPages(handle));
    removeAvailRun(_runsAvail[pageIdxFloor], handle);
}

void Chunk::removeAvailRun(HandlePriorityQueue &queue, handle_t handle)
{
    queue.remove(handle);

    int offset = runOffset(handle);
    int pages = runPages(handle);
    //remove first page of run
    _runsAvailMap.erase(offset);
    if (pages > 1)
    {
        //remove last page of run
        _runsAvailMap.erase(lastPage(offset, pages));
    }
}

int Chunk::lastPage(offset_t runOffset, int pages)
{
    return runOffset + pages - 1;
}

int Chunk::runPages(handle_t handle)
{
    //handle_t: oooooooo ooooooos ssssssss ssssssue bbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb
    //需要得到s ssssssss ssssss
    return (int)(handle >> SIZE_SHIFT & 0x7fff);
}

int Chunk::runOffset(handle_t handle)
{
    //handle_t: oooooooo ooooooos ssssssss ssssssue bbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb
    //需要得到oooooooo ooooooo
    return (int)(handle >> RUN_OFFSET_SHIFT);
}

int Chunk::pages2pageIdx(int pages)
{
    // return pages2pageIdxCompute(pages, false);
    return 0;
}

int Chunk::pages2pageIdxFloor(int pages)
{
    // return pages2pageIdxCompute(pages, _pageShifts, _chunkSize, true);
    return 0;
}

int Chunk::pages2pageIdxCompute(int pages, int pageShifts, int chunkSize, bool floor)
{
    // int pageSize = pages << pageShifts;
    // if (pageSize > chunkSize)
    // {
    //     return nPSizes;
    // }

    // int x = log2((pageSize << 1) - 1);

    // int shift = x < LOG2_SIZE_CLASS_GROUP + pageShifts
    //                 ? 0
    //                 : x - (LOG2_SIZE_CLASS_GROUP + pageShifts);

    // int group = shift << LOG2_SIZE_CLASS_GROUP;

    // int log2Delta = x < LOG2_SIZE_CLASS_GROUP + pageShifts + 1 ? pageShifts : x - LOG2_SIZE_CLASS_GROUP - 1;

    // int deltaInverseMask = -1 << log2Delta;
    // int mod = (pageSize - 1 & deltaInverseMask) >> log2Delta &
    //           (1 << LOG2_SIZE_CLASS_GROUP) - 1;

    // int pageIdx = group + mod;

    // if (floor && pageIdx2sizeTab[pageIdx] > pages << pageShifts)
    // {
    //     pageIdx--;
    // }

    // return pageIdx;
    return 0;
}