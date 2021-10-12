#include <cassert>

#include "pool/size_classes.hpp"
#include "pool/subpage.hpp"
#include "pool/util.hpp"

Subpage::Subpage()
    : _chunk(nullptr), _pageShifts(-1), _runOffset(-1), _runSize(-1), _bitmap(nullptr), _elemSize(-1)
{
}

Subpage::Subpage(Subpage *head, Chunk *chunk, int pageShifts, int runOffset, int runSize, int elemSize)
    : _chunk(chunk), _pageShifts(pageShifts), _runOffset(runOffset), _runSize(runSize),
      _bitmap(new std::int64_t[((std::uint64_t)runSize >> 6) + SizeClasses::LOG2_QUANTUM]),
      _elemSize(elemSize), _doNotDestroy(true)

{
    if (_elemSize != 0)
    {
        _maxNumElems = _numAvail = runSize / elemSize;
        _nextAvail = 0;
        _bitmapLength = (unsigned int)_maxNumElems >> 6;
        if ((_maxNumElems & 63) != 0)
        {
            _bitmapLength++;
        }

        for (int i = 0; i < _bitmapLength; i++)
        {
            _bitmap[i] = 0;
        }
    }
    addToPool(head);
}

Subpage::~Subpage()
{
    if(_bitmap != nullptr)
    {
        delete _bitmap;
    }
}

std::int64_t Subpage::allocate()
{
    if (_numAvail == 0 || !_doNotDestroy)
    {
        return -1;
    }

    int bitmapIdx = getNextAvail();
    int q = logic_rshift<int>(bitmapIdx, 6);
    int r = bitmapIdx & 63;
    assert((logic_rshift<std::int64_t>(_bitmap[q], r) & 1) == 0);
    _bitmap[q] |= 1L << r;

    if (--_numAvail == 0)
    {
        removeFromPool();
    }

    return toHandle(bitmapIdx);
}

void Subpage::addToPool(Subpage *head)
{
    assert(_prev == nullptr && _next == nullptr);
    _prev = head;
    _next = head->_next;
    _next->_prev = this;
    head->_next = this;
}

int Subpage::getNextAvail()
{
    int nextAvail = this->_nextAvail;
    if (nextAvail >= 0)
    {
        this->_nextAvail = -1;
        return nextAvail;
    }
    return findNextAvail();
}

void Subpage::removeFromPool()
{
    assert(_prev != nullptr && _next != nullptr);
    _prev->_next = _next;
    _next->_prev = _prev;
    _next = nullptr;
    _prev = nullptr;
}

Chunk::handle_t Subpage::toHandle(int bitmapIdx)
{
    int pages = _runSize >> _pageShifts;
    return (std::int64_t)_runOffset << Chunk::RUN_OFFSET_SHIFT | (std::int64_t)pages << Chunk::SIZE_SHIFT | 1L << Chunk::IS_USED_SHIFT | 1L << Chunk::IS_SUBPAGE_SHIFT | bitmapIdx;
}

int Subpage::findNextAvail()
{
    auto bitmap = this->_bitmap;
    auto bitmapLength = this->_bitmapLength;
    for (int i = 0; i < bitmapLength; i++)
    {
        long bits = bitmap[i];
        if (~bits != 0)
        {
            return findNextAvail0(i, bits);
        }
    }
    return -1;
}

int Subpage::findNextAvail0(int i, std::int64_t bits)
{
    int maxNumElems = this->_maxNumElems;
    int baseVal = i << 6;

    for (int j = 0; j < 64; j++)
    {
        if ((bits & 1) == 0)
        {
            int val = baseVal | j;
            if (val < maxNumElems)
            {
                return val;
            }
            else
            {
                break;
            }
        }
        bits = logic_rshift<std::int64_t>(bits, 1);
    }
    return -1;
}

bool Subpage::free(Subpage *head, int bitmapIdx)
{
    if (_elemSize == 0)
    {
        return true;
    }
    int q = logic_rshift<int>(bitmapIdx, 6);
    int r = bitmapIdx & 63;
    assert((logic_rshift<std::int64_t>(_bitmap[q], r) & 1) != 0);
    _bitmap[q] ^= 1L << r;

    setNextAvail(bitmapIdx);

    if (_numAvail++ == 0)
    {
        addToPool(head);
        /* When maxNumElems == 1, the maximum numAvail is also 1.
             * Each of these PoolSubpages will go in here when they do free operation.
             * If they return true directly from here, then the rest of the code will be unreachable
             * and they will not actually be recycled. So return true only on maxNumElems > 1. */
        if (_maxNumElems > 1)
        {
            return true;
        }
    }

    if (_numAvail != _maxNumElems)
    {
        return true;
    }
    else
    {
        // Subpage not in use (numAvail == maxNumElems)
        if (_prev == _next)
        {
            // Do not remove if this subpage is the only one left in the pool.
            return true;
        }

        // Remove this subpage from the pool if there are other subpages left in the pool.
        _doNotDestroy = false;
        removeFromPool();
        return false;
    }
}

void Subpage::setNextAvail(int bitmapIdx)
{
    _nextAvail = bitmapIdx;
}

int Subpage::pageSize()
{
    return 1 << _pageShifts;
}

void Subpage::destroy()
{
    if (_chunk != nullptr)
    {
        // _chunk->destroy();
    }
}