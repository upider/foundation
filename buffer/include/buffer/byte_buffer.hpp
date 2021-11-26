#ifndef __BYTE_BUFFER_HPP__
#define __BYTE_BUFFER_HPP__

#include <cstring>
#include <cinttypes>

template <typename T>
class ByteBuffer
{
protected:
    std::size_t _widx = 0;
    std::size_t _ridx = 0;
    std::size_t _cap = 0;
    T *_bytes = nullptr;

    void recreate_bytes(std::size_t cap);

public:
    ByteBuffer();
    ByteBuffer(const ByteBuffer &other);
    ByteBuffer(const T *bytes, std::size_t len);
    ByteBuffer(const T *bytes, std::size_t len, std::size_t cap);
    ByteBuffer(std::size_t cap);
    virtual ~ByteBuffer();

    ByteBuffer &operator=(const ByteBuffer &other);
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
    std::string str();
    /**
     * @brief 克隆一个新的ByteBuffer
     * 
     * @return ByteBuffer 新的ByteBuffer
     */
    ByteBuffer clone();
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

    template <typename TT>
    std::size_t fill(const TT &ch, std::size_t len);
    template <typename TT>
    std::size_t fill(TT &&ch, std::size_t len);
    template <typename TT>
    TT read(std::size_t index);
    template <typename TT>
    std::size_t write(const TT &ele);
    template <typename TT>
    std::size_t write(TT &&ele);
    template <typename TT>
    std::size_t read(TT *dst, std::size_t size);
    template <typename TT>
    std::size_t write(TT *dst, std::size_t size);
};

template<typename T>
void ByteBuffer<T>::recreate_bytes(std::size_t cap) 
{
    if (_bytes != nullptr)
    {
        delete[] _bytes;
    }
    _bytes = new T[cap];
}

template <typename T>
ByteBuffer<T>::ByteBuffer() {}

template <typename T>
ByteBuffer<T>::ByteBuffer(const ByteBuffer &other)
{
    recreate_bytes(other._cap);
    _widx = other._widx;
    _ridx = other._ridx;
    _cap = other._cap;
    std::memcpy(_bytes, other._bytes, other._cap);
}

template <typename T>
ByteBuffer<T>::ByteBuffer(const T *bytes, std::size_t len)
{
    recreate_bytes(len);
    _ridx = 0;
    _widx = len;
    _cap = len;
    std::memcpy(_bytes, bytes, len);
}

template <typename T>
ByteBuffer<T>::ByteBuffer(const T *bytes, std::size_t len, std::size_t cap)
{
    recreate_bytes(cap);
    _ridx = 0;
    _widx = len;
    _cap = cap;
    std::memcpy(_bytes, bytes, len);
}

template <typename T>
ByteBuffer<T>::ByteBuffer(std::size_t cap)
{
    recreate_bytes(cap);
    _ridx = 0;
    _widx = 0;
    _cap = cap;
}

template <typename T>
ByteBuffer<T>::~ByteBuffer()
{
    if (_bytes != nullptr)
    {
        delete[] _bytes;
    }
}

template<typename T>
ByteBuffer<T>& ByteBuffer<T>::operator=(const ByteBuffer &other) 
{
    recreate_bytes(other._cap);
    _widx = other._widx;
    _ridx = other._ridx;
    _cap = other._cap;
    std::memcpy(_bytes, other._bytes, other._cap);
}

template<typename T>
std::size_t ByteBuffer<T>::cap() 
{
    return _cap;
}

template<typename T>
std::size_t ByteBuffer<T>::writable() 
{
    return _cap - _widx;
}

template<typename T>
std::size_t ByteBuffer<T>::readable() 
{
    return _widx - _ridx;
}

template<typename T>
std::string ByteBuffer<T>::str() 
{
    return std::string((char*)_bytes);
}

template<typename T>
ByteBuffer<T> ByteBuffer<T>::clone() 
{
    return ByteBuffer(*this);
}

template<typename T>
T* ByteBuffer<T>::data() 
{
    return _bytes;
}

template<typename T>
const T* ByteBuffer<T>::const_data() const
{
    return _bytes;
}

template<typename T>
void ByteBuffer<T>::discard_read_bytes() 
{
    //TODO
    // std::memcpy(_bytes, );
}

template<typename T>
void ByteBuffer<T>::reset(const T *bytes, std::size_t len, std::size_t cap) 
{
    recreate_bytes(cap);
    _widx = len;
    _ridx = 0;
    std::memcpy(_bytes, bytes, cap);
}

#endif /* __BYTE_BUFFER_HPP__ */
