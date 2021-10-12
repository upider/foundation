#ifndef __SUBPAGE_HPP__
#define __SUBPAGE_HPP__

#include <bitset>

#include "pool/chunk.hpp"

//记录每个page被分割情况
class Subpage
{
private:
    const Chunk *_chunk;
    const int _pageShifts;
    const int _runOffset;
    const int _runSize;
    std::int64_t *_bitmap;

    Subpage *_prev;
    Subpage *_next;

    int _elemSize;
    bool _doNotDestroy;
    int _maxNumElems;
    int _bitmapLength;
    int _nextAvail;
    int _numAvail;

private:
    void addToPool(Subpage *head);
    int getNextAvail();
    void removeFromPool();
    Chunk::handle_t toHandle(int bitmapIdx);
    int findNextAvail();
    int findNextAvail0(int i, std::int64_t bits);

public:
    Subpage();
    Subpage(Subpage *head, Chunk *chunk, int pageShifts, int runOffset, int runSize, int elemSize);
    ~Subpage();

    //Returns the bitmap index of the subpage allocation.
    std::int64_t allocate();
    bool free(Subpage *head, int bitmapIdx);
    /**
     * @return {@code true} if this subpage is in use.
     *         {@code false} if this subpage is not used by its chunk and thus it's OK to be released.
     */
    void setNextAvail(int bitmapIdx);
    int pageSize();
    void destroy();
};

#endif // __SUBPAGE_HPP__