#ifndef __SELECTABLE_HPP__
#define __SELECTABLE_HPP__

#include <sys/epoll.h>

#include <cinttypes>
#include <chrono>

class Selectable
{
public:
    friend class Acceptor;
    typedef int native_handle_type;
    typedef uint32_t OperationCollection;
    enum Operation
    {
        OP_READ = EPOLLIN,
        OP_WRITE = EPOLLOUT,
        OP_ONESHOT = EPOLLONESHOT,
        OP_REMOTECLOSE = EPOLLRDHUP,
        OP_EXCEPT = EPOLLHUP|EPOLLERR
    };

    enum Type
    {
        SERVER_STREAM_SOCKET,
        STREAM_SOCKET,
        DATAGRAM_SOCKET,
    };

    Selectable(const Selectable& other);
    ~Selectable();
    const Type type() const;
    const native_handle_type native_handle() const;
    OperationCollection valid_operations();
    bool non_block();
    void non_block(bool non_block);
    /**
     * @brief 关闭Selectable对象，关闭底层文件描述符
     */
    void close();
    /**
     * @brief 停止Selectable一种或多种操作(OP_READ | OP_WRITE | OP_ACCEPT)
     * 
     * @param ops 要停止的操作
     */
    void shutdown(OperationCollection ops);
    bool operator==(const Selectable& selectable);
    bool closed();

    static Selectable open_stream_socket(bool non_block = true);
    static Selectable open_server_stream_socket(bool non_block = true);
    static Selectable open_datagram_socket(bool non_block = true);
    template <typename Rep, typename Period>
    static Selectable open_timer(const std::chrono::duration<Rep, Period> &duration, bool non_block = true);

private:
    native_handle_type _native_handle = -1;
    Type _type;
    bool _non_block;
    bool _closed = false;
    Selectable();
    Selectable(const native_handle_type& h, Type t, bool non_block);
};

#endif /* __SELECTABLE_HPP__ */
