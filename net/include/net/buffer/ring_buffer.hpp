#ifndef __RING_BUFFER_HPP__
#define __RING_BUFFER_HPP__

#include <cinttypes>

namespace net
{
    namespace buffer
    {
        /**
         * @brief 用于socket的write buffer
         * 
         * @tparam T 元素类型
         * @tparam Alloc 内存分配器
         */
        template <typename T, typename Alloc>
        class RingBuffer
        {
        public:
            typedef T value_type;
            typedef T *pointer;
            typedef Alloc allocator_type;

        private:
            T *_buffer;
            std::size_t _head;
            std::size_t _tail;
            std::size_t _max; //of the buffer
            bool _full;
            Alloc _allocator;

        public:
            RingBuffer(std::size_t size);
            ~RingBuffer();

            void clear();
            std::size_t cap() const;
            std::size_t size() const;
            bool empty() const;
            bool full() const;
            T *data();
            const T *const_data() const;
        };

        template <typename T, typename Alloc>
        RingBuffer<T, Alloc>::RingBuffer(std::size_t size) : _max(size)
        {
            _buffer = _allocator.allocate(size);
        }

        template <typename T, typename Alloc>
        RingBuffer<T, Alloc>::~RingBuffer()
        {
            _allocator.deallocate(_buffer, _max);
        }

        template <typename T, typename Alloc>
        void RingBuffer<T, Alloc>::clear()
        {
            _head = 0;
            _tail = 0;
        }

        template <typename T, typename Alloc>
        std::size_t RingBuffer<T, Alloc>::cap() const
        {
            return _max;
        }

        template <typename T, typename Alloc>
        std::size_t RingBuffer<T, Alloc>::size() const
        {
            size_t size = _max;

            if (!_full)
            {
                if (_head >= _tail)
                {
                    size = _head - _tail;
                }
                else
                {
                    size = _max + _head - _tail;
                }
            }

            return size;
        }

        template <typename T, typename Alloc>
        bool RingBuffer<T, Alloc>::empty() const
        {
            return (!_full && (_head == _tail));
        }

        template <typename T, typename Alloc>
        bool RingBuffer<T, Alloc>::full() const
        {
            return _full;
        }

        template <typename T, typename Alloc>
        T *RingBuffer<T, Alloc>::data()
        {
            return _buffer;
        }

        template <typename T, typename Alloc>
        T *RingBuffer<T, Alloc>::data()
        {
            return _buffer;
        }

        template <typename T, typename Alloc>
        const T *RingBuffer<T, Alloc>::const_data() const
        {
            return _buffer;
        }
        
    } // namespace buffer
} // namespace net

#endif /* __RING_BUFFER_HPP__ */
