#ifndef __NET_BASIC_BUFFER_HPP__
#define __NET_BASIC_BUFFER_HPP__

namespace net
{
    namespace buffer
    {
        template<typename T, typename Alloc>
        class BasicBuffer
        {
        private:
        public:
            BasicBuffer();
            ~BasicBuffer();
        };

    } // namespace buffer
} // namespace net
#endif /* __NET_BASIC_BUFFER_HPP__ */
