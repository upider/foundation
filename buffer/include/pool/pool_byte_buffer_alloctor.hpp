#ifndef __POOL_BYTE_BUFFER_ALLOCTOR_HPP__
#define __POOL_BYTE_BUFFER_ALLOCTOR_HPP__

#include <memory>

class ByteBuffer;

class PoolByteBufferAlloctor
{
public:
    PoolByteBufferAlloctor();
    ~PoolByteBufferAlloctor();

public:
    ByteBuffer *buffer(std::size_t size);
};

#endif // __POOL_BYTE_BUFFER_ALLOCTOR_HPP__