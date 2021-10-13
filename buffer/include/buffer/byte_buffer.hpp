#ifndef __BYTE_BUFFER_HPP__
#define __BYTE_BUFFER_HPP__

#include <cinttypes>

class Chunk;
class ThreadCache;

class ByteBuffer
{
private:
    friend class Arena;

    int _maxCapacity;
    Chunk *chunk;
    std::int64_t handle;
    int offset;
    int length;
    int maxLength;
    ThreadCache *cache;

public:
    ByteBuffer();
    ~ByteBuffer();
};

#endif // __BYTE_BUFFER_HPP__