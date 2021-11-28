#ifndef __BASIC_BYTE_BUFFER_HPP__
#define __BASIC_BYTE_BUFFER_HPP__

#include <cstring>
#include <cinttypes>
#include <memory>

#include "byte/byte.hpp"
#include "buffer/type_traits.hpp"

template <typename T, typename Alloc, typename Enable = void>
class BasicByteBuffer
{
};

template <typename T, typename Alloc>
class BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>
{
protected:
    std::size_t _widx = 0;
    std::size_t _ridx = 0;
    std::size_t _cap = 0;
    T *_bytes = nullptr;
    Alloc _allocator;

    void recreate_data(std::size_t cap);

public:
    typedef T value_type;
    typedef T *pointer;
    typedef Alloc allocator_type;

public:
    BasicByteBuffer(Alloc a = Alloc());
    BasicByteBuffer(const BasicByteBuffer &other);
    BasicByteBuffer(const T *bytes, std::size_t len, Alloc a = Alloc());
    BasicByteBuffer(const T *bytes, std::size_t len, std::size_t cap, Alloc a = Alloc());
    BasicByteBuffer(std::size_t cap, Alloc a = Alloc());
    virtual ~BasicByteBuffer();

    template <typename T2, typename Alloc2>
    BasicByteBuffer &operator=(const BasicByteBuffer<T2, Alloc2> &other);
    /**
     * @brief 得到容量
     * 
     * @return std::size_t  容量
     */
    std::size_t cap();
    /**
     * @brief 可写大小
     * 
     * @return std::size_t 可写大小
     */
    std::size_t writable();
    /**
     * @brief 可读大小
     * 
     * @return std::size_t 可读大小
     */
    std::size_t readable();
    /**
     * @brief 将buffer内容转换成string
     * 
     * @return std::string buffer内容
     */
    std::string to_str();
    /**
     * @brief 将buffer内容转换成string
     * 
     * @param index 起始下标
     * @param n 元素个数
     * @return std::string buffer内容
     */
    std::string str(std::size_t index, std::size_t n);
    /**
     * @brief 克隆一个新的BasicByteBuffer
     * 
     * @return BasicByteBuffer 新的BasicByteBuffer
     */
    BasicByteBuffer clone();
    /**
     * @brief 得到buffer的原始指针
     * 
     * @return Byte* buffer的原始指针
     */
    T *data();
    /**
     * @brief 得到buffer的原始指针
     * 
     * @return Byte* buffer的原始指针
     */
    const T *const_data() const;
    /**
     * @brief 丢弃已经读取的bytes, 并将buffer内容copy到开始位置
     */
    void discard_read_bytes();
    /**
     * @brief 重置byte指针
     * 
     * @param bytes byte指针
     * @param len 数据长度
     * @param cap 最大容量
     */
    void reset(const T *bytes, std::size_t len, std::size_t cap);
    /**
     * @brief 重置byte指针, 容量设置为len
     * 
     * @param bytes byte指针
     * @param len 数据长度, 容量
     */
    void reset(const T *bytes, std::size_t len);
    /**
     * @brief 清空buffer
     * 
     */
    void reset();
    /**
     * @brief 从read index位置开始, 使用符号填充buffer
     * 
     * @param ch 使用的符号
     * @param len 填充长度
     * @return std::size_t 成功填充长度, 剩下的可写空间可能小于参数len
     */
    std::size_t fill(const T &ch, std::size_t len);
    /**
     * @brief 从read index位置开始, 使用符号填充buffer
     * 
     * @param ch 使用的符号
     * @param len 填充长度
     * @return std::size_t 成功填充长度, 剩下的可写空间可能小于参数len
     */
    std::size_t fill(T &&ch, std::size_t len);
    /**
     * @brief 获取在read index + i 位置的元素引用
     * 
     * @param i 要获取的位置
     * @return T& 元素引用
     */
    T &operator[](std::size_t i);
    /**
     * @brief 从index位置读取T2类型元素, 不增加read index, 有可能读取超过可读数量的字节或者读取到buffer外内存
     * 
     * @tparam T2 读取元素类型, 可以是char,u_char,数字
     * @tparam N T2大小
     * @param index 开始位置
     * @return T2 元素
     */
    template <typename T2, std::size_t N = sizeof(T2)>
    typename is_readable<T2>::type read(std::size_t index);
    /**
     * @brief 从index位置读取T2类型元素, 并增加read index, 有可能读取超过可读数量的字节或读取到buffer外内存
     * 
     * @tparam T2 读取元素类型, 可以是char,u_char,数字
     * @tparam N T2大小
     * @param index 开始位置
     * @return T2 元素
     */
    template <typename T2, std::size_t N = sizeof(T2)>
    typename is_readable<T2>::type read();
    /**
     * @brief 写入元素到buffer指定位置, 若超过write index则增加write index, 若元素过大写到buffer外内存, 元素会被截断
     * 
     * @tparam T2 写入元素元素类型
     * @param ele 要写入元素
     * @tparam N T2大小
     * @param index 开始位置
     * @return std::size_t 写入字节数, 如果写入到buffer内存外的位置则可能返回小于sizeof(ele)的值
     */
    template <typename T2, std::size_t N = sizeof(T2)>
    std::size_t write(const typename is_readable<T2>::type &ele, std::size_t index);
    /**
     * @brief 追加写入元素, 增加write index, 返回真正写入字节数, 如果元素过大导致超过buffer容量元素会被截断
     * 
     * @tparam T2 写入元素元素类型
     * @tparam N 写入元素大小
     * @param ele 要写入元素
     * @return std::size_t 写入字节数, 如果写入到buffer内存外de位置则可能返回小于sizeof(ele)的值
     */
    template <typename T2, std::size_t N = sizeof(T2)>
    std::size_t write(const typename is_readable<T2>::type &ele);
    /**
     * @brief 读取数据到dst, 增加read index
     * 
     * @tparam T2 读取元素类型, 可以是char, u_char, integral
     * @param dst 目标地址
     * @param size 长度
     * @return std::size_t 读取到的长度, 可能小于指定长度
     */
    template <typename T2>
    std::size_t read(typename is_readable<T2>::type *dst, std::size_t size);
    /**
     * @brief 追加写入元素到buffer, 最多写入可写大小
     * 
     * @tparam T2 元素类型
     * @param src 内存位置
     * @param size 写入大小
     * @return std::size_t 真正写入大小
     */
    template <typename T2>
    std::size_t write(const typename is_readable<T2>::type *src, std::size_t size);
};

template <typename T, typename Alloc>
void BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::recreate_data(std::size_t cap)
{
    if (_bytes != nullptr)
    {
        _allocator.deallocate(_bytes, cap);
    }
    _bytes = _allocator.allocate(cap);
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::BasicByteBuffer(Alloc a) : _allocator(a) {}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::BasicByteBuffer(const BasicByteBuffer &other)
{
    _allocator = other._allocator;
    recreate_data(other._cap);
    _widx = other._widx;
    _ridx = other._ridx;
    _cap = other._cap;
    std::memcpy(_bytes, other._bytes, other._cap);
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::BasicByteBuffer(const T *bytes, std::size_t len, Alloc a)
{
    _allocator = a;
    recreate_data(len);
    _ridx = 0;
    _widx = len;
    _cap = len;
    std::memcpy(_bytes, bytes, len);
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::BasicByteBuffer(const T *bytes, std::size_t len, std::size_t cap, Alloc a)
{
    _allocator = a;
    recreate_data(cap);
    _ridx = 0;
    _widx = len;
    _cap = cap;
    std::memcpy(_bytes, bytes, len);
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::BasicByteBuffer(std::size_t cap, Alloc a)
{
    _allocator = a;
    recreate_data(cap);
    _ridx = 0;
    _widx = 0;
    _cap = cap;
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::~BasicByteBuffer()
{
    if (_bytes != nullptr)
    {
        _allocator.deallocate(_bytes, _cap);
    }
}

template <typename T, typename Alloc>
template <typename T2, typename Alloc2>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type> &
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::operator=(const BasicByteBuffer<T2, Alloc2> &other)
{
    recreate_data(other._cap);
    _widx = other._widx;
    _ridx = other._ridx;
    _cap = other._cap;
    std::memcpy(_bytes, other._bytes, other._cap);
}

template <typename T, typename Alloc>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::cap()
{
    return _cap;
}

template <typename T, typename Alloc>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::writable()
{
    return _cap - _widx;
}

template <typename T, typename Alloc>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::readable()
{
    return _widx - _ridx;
}

template <typename T, typename Alloc>
std::string BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::to_str()
{
    return std::string((char *)_bytes + _ridx, (char *)_bytes + _widx);
}

template <typename T, typename Alloc>
std::string BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::str(std::size_t index, std::size_t n)
{
    return std::string((char *)_bytes + index, (char *)_bytes + index + n);
}

template <typename T, typename Alloc>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::clone()
{
    return BasicByteBuffer(*this);
}

template <typename T, typename Alloc>
T *BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::data()
{
    return _bytes;
}

template <typename T, typename Alloc>
const T *BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::const_data() const
{
    return _bytes;
}

template <typename T, typename Alloc>
void BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::discard_read_bytes()
{
    std::memmove(_bytes, _bytes + _ridx, readable());
    _ridx = 0;
    _widx -= _ridx;
}

template <typename T, typename Alloc>
void BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::reset(const T *bytes, std::size_t len, std::size_t cap)
{
    recreate_data(cap);
    _widx = len;
    _ridx = 0;
    _cap = cap;
    std::memcpy(_bytes, bytes, cap);
}

template <typename T, typename Alloc>
void BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::reset(const T *bytes, std::size_t len)
{
    recreate_data(len);
    _widx = len;
    _ridx = 0;
    _cap = len;
    std::memcpy(_bytes, bytes, len);
}

template <typename T, typename Alloc>
void BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::reset()
{
    _widx = 0;
    _ridx = 0;
}

template <typename T, typename Alloc>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::fill(const T &ch, std::size_t len)
{
    std::size_t res = writable();
    if (len < res)
    {
        res = len;
    }

    std::fill_n(_bytes + _widx, res, ch);
    _widx += res;

    return res;
}

template <typename T, typename Alloc>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::fill(T &&ch, std::size_t len)
{
    std::size_t res = writable();
    if (len < res)
    {
        res = len;
    }

    std::fill_n(_bytes + _widx, res, ch);
    _widx += res;

    return res;
}

template <typename T, typename Alloc>
T &BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::operator[](std::size_t index)
{
    return _bytes[_ridx + index];
}

template <typename T, typename Alloc>
template <typename T2>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::write(const typename is_readable<T2>::type *src, std::size_t size)
{
    std::size_t wsize = writable();
    if (size > wsize)
    {
        std::memcpy(_bytes + _widx, src, wsize);
        _widx = _cap;
        return wsize;
    }
    else
    {
        std::memcpy(_bytes + _widx, src, size);
        _widx += size;
        return size;
    }
}

template <typename T, typename Alloc>
template <typename T2>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::read(typename is_readable<T2>::type *dst, std::size_t size)
{
    std::size_t rsize = readable();
    if (size >= rsize)
    {
        std::memcpy(dst, _bytes + _ridx, rsize);
        _ridx = _widx;
        reset();
        return rsize;
    }
    else
    {
        std::memcpy(dst, _bytes + _ridx, size);
        _ridx += size;
        return size;
    }
}

template <typename T, typename Alloc>
template <typename T2, std::size_t N>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::write(const typename is_readable<T2>::type &ele)
{
    std::size_t wsize = writable();
    if (N > wsize)
    {
        std::memcpy(_bytes + _widx, &ele, wsize);
        _widx = _cap;
        return wsize;
    }
    else
    {
        return write<T2>(&ele, N);
    }
}

template <typename T, typename Alloc>
template <typename T2, std::size_t N>
std::size_t BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::write(const typename is_readable<T2>::type &ele, std::size_t index)
{
    std::size_t wsize = writable();
    if (N + index > wsize)
    {
        //写入位置超过可写大小, 截断写入
        std::memcpy(_bytes + index, &ele, wsize);
        _widx = _cap;
        return wsize;
    }
    else
    {
        std::memcpy(_bytes + index, &ele, N);
        _widx += N;
        return wsize;
    }
}

template <typename T, typename Alloc>
template <typename T2, std::size_t N>
typename is_readable<T2>::type
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::read(std::size_t index)
{
    T2 res = (T2)(*(_bytes + _ridx + index));
    return res;
}

template <typename T, typename Alloc>
template <typename T2, std::size_t N>
typename is_readable<T2>::type
BasicByteBuffer<T, Alloc, typename std::enable_if<sizeof(T) == 1>::type>::read()
{

    T2 res = (T2)(*(_bytes + _ridx));
    _ridx += N;
    if (_ridx > _widx)
    {
        _ridx = _widx;
    }
    return res;
}

#endif /* __BASIC_BYTE_BUFFER_HPP__ */
