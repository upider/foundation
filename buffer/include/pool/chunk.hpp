#ifndef __CHUNK_HPP__
#define __CHUNK_HPP__

/**
 * Description of algorithm for PageRun/PoolSubpage allocation from PoolChunk
 *
 * Notation: The following terms are important to understand the code
 * > page  - a page is the smallest unit of memory chunk that can be allocated
 * > run   - a run is a collection of pages
 * > chunk - a chunk is a collection of runs
 * > in this code chunkSize = maxPages * pageSize
 *
 * To begin we allocate a byte array of size = chunkSize
 * Whenever a ByteBuf of given size needs to be created we search for the first position
 * in the byte array that has enough empty space to accommodate the requested size and
 * return a (long) handle that encodes this offset information, (this memory segment is then
 * marked as reserved so it is always used by exactly one ByteBuf and no more)
 *
 * For simplicity all sizes are normalized according to {@link PoolArena#size2SizeIdx(int)} method.
 * This ensures that when we request for memory segments of size > pageSize the normalizedCapacity
 * equals the next nearest size in {@link SizeClasses}.
 *
 *
 *  A chunk has the following layout:
 *
 *     /-----------------\
 *     | run             |
 *     |                 |
 *     |                 |
 *     |-----------------|
 *     | run             |
 *     |                 |
 *     |-----------------|
 *     | unalloctated    |
 *     | (freed)         |
 *     |                 |
 *     |-----------------|
 *     | subpage         |
 *     |-----------------|
 *     | unallocated     |
 *     | (freed)         |
 *     | ...             |
 *     | ...             |
 *     | ...             |
 *     |                 |
 *     |                 |
 *     |                 |
 *     \-----------------/
 *
 *
 * handle:
 * -------
 * a handle is a long number, the bit layout of a run looks like:
 *
 * oooooooo ooooooos ssssssss ssssssue bbbbbbbb bbbbbbbb bbbbbbbb bbbbbbbb
 *
 * o: runOffset (page offset in the chunk), 15bit
 * s: size (number of pages) of this run, 15bit
 * u: isUsed?, 1bit
 * e: isSubpage?, 1bit
 * b: bitmapIdx of subpage, zero if it's not subpage, 32bit
 *
 * runsAvailMap:
 * ------
 * a map which manages all runs (used and not in used).
 * For each run, the first runOffset and last runOffset are stored in runsAvailMap.
 * key: runOffset
 * value: handle
 *
 * runsAvail:
 * ----------
 * an array of {@link PriorityQueue}.
 * Each queue manages same size of runs.
 * Runs are sorted by offset, so that we always allocate runs with smaller offset.
 *
 *
 * Algorithm:
 * ----------
 *
 *   As we allocate runs, we update values stored in runsAvailMap and runsAvail so that the property is maintained.
 *
 * Initialization -
 *  In the beginning we store the initial run which is the whole chunk.
 *  The initial run:
 *  runOffset = 0
 *  size = chunkSize
 *  isUsed = no
 *  isSubpage = no
 *  bitmapIdx = 0
 *
 *
 * Algorithm: [allocateRun(size)]
 * ----------
 * 1) find the first avail run using in runsAvails according to size
 * 2) if pages of run is larger than request pages then split it, and save the tailing run
 *    for later using
 *
 * Algorithm: [allocateSubpage(size)]
 * ----------
 * 1) find a not full subpage according to size.
 *    if it already exists just return, otherwise allocate a new PoolSubpage and call init()
 *    note that this subpage object is added to subpagesPool in the PoolArena when we init() it
 * 2) call subpage.allocate()
 *
 * Algorithm: [free(handle, length, nioBuffer)]
 * ----------
 * 1) if it is a subpage, return the slab back into this subpage
 * 2) if the subpage is not used or it is a run, then start free this run
 * 3) merge continuous avail runs
 * 4) save the merged run
 *
 */

#include <cinttypes>
#include <functional>
#include <map>
#include <queue>
#include <vector>

class Arena;
class Subpage;
template <typename T>
class PriorityQueue;

class Chunk
{
private:
    static const int SIZE_BIT_LENGTH = 15;
    static const int INUSED_BIT_LENGTH = 1;
    static const int SUBPAGE_BIT_LENGTH = 1;
    static const int BITMAP_IDX_BIT_LENGTH = 32;

public:
    static const int IS_SUBPAGE_SHIFT = BITMAP_IDX_BIT_LENGTH;
    static const int IS_USED_SHIFT = SUBPAGE_BIT_LENGTH + IS_SUBPAGE_SHIFT;
    static const int SIZE_SHIFT = INUSED_BIT_LENGTH + IS_USED_SHIFT;
    static const int RUN_OFFSET_SHIFT = SIZE_BIT_LENGTH + SIZE_SHIFT;

    using handle_t = std::int64_t;
    using offset_t = int;

    std::function<bool(handle_t, handle_t)> handleGreater = [](handle_t l, handle_t r) -> bool
    {
        return l > r;
    };
    using HandlePriorityQueue = PriorityQueue<handle_t>;

private:
    Arena *_arena;
    int _pageSize;
    int _pageShifts;
    int _chunkSize;
    using Page = page_t;
    Page *_pages;

    //store the first page and last page of each avail run
    std::map<offset_t, handle_t> _runsAvailMap;
    // manage all avail runs
    std::vector<HandlePriorityQueue> _runsAvail;
    std::vector<Subpage *> _subpages;

public:
    Chunk(Arena *arena, int pageSize, int pageShifts, int chunkSize, int maxPageIdx);
    ~Chunk();

    //计算page index
    static int pages2pageIdx(int pages);
    static int pages2pageIdxFloor(int pages);
    static int pages2pageIdxCompute(int pages, int pageShifts, int chunkSize, bool floor);

private:
    //插入一个run
    void insertAvailRun(offset_t runOffset, int pages, handle_t handle);
    void insertAvailRun0(offset_t runOffset, handle_t handle);
    //删除一个run
    void removeAvailRun(handle_t handle);
    void removeAvailRun(HandlePriorityQueue &queue, handle_t handle);
    //得到最后一页位置
    static int lastPage(offset_t runOffset, int pages);
    //get number of pages, 15bit
    static int runPages(handle_t handle);
    //get page offset in the chunk, 15bit
    static int runOffset(handle_t handle);
    
};

#endif // __CHUNK_HPP__