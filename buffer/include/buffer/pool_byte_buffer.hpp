#ifndef __POOL_BYTE_BUFFER_HPP__
#define __POOL_BYTE_BUFFER_HPP__

#include <memory>
#include <thread>

#include "buffer/byte_buffer.hpp"
#include "buffer/pool/constant.hpp"
#include "buffer/pool/pool_byte_buffer_allocator.hpp"

typedef BasicByteBuffer<Byte, PoolByteBufferAllocator<Byte>> PoolByteBuffer;

#endif /* __POOL_BYTE_BUFFER_HPP__ */
