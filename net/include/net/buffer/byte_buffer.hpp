#ifndef __NET_BYTE_BUFFER_HPP__
#define __NET_BYTE_BUFFER_HPP__

#include <cinttypes>

namespace net
{
    namespace buffer
    {
        class Byte;
        // template<typename Alloc>
        class ByteBuffer
        {
        public:
            ByteBuffer();
            ~ByteBuffer();

            std::size_t size();
            Byte* data();
        };
    
    } // namespace buffer
} // namespace net


#endif /* __NET_BYTE_BUFFER_HPP__ */
