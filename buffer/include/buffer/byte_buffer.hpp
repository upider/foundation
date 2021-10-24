#ifndef __BYTE_BUFFER_HPP__
#define __BYTE_BUFFER_HPP__

#include <cinttypes>

class ByteBuffer
{
public:
    ByteBuffer();
    ByteBuffer(std::size_t maxLength);
    virtual ~ByteBuffer();
};

#endif // __BYTE_BUFFER_HPP__