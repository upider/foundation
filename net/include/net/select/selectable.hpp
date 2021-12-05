#ifndef __SELECTABLE_HPP__
#define __SELECTABLE_HPP__

#include <sys/epoll.h>

#include <cinttypes>
#include <chrono>

class Selectable
{
public:
    typedef int native_handle_type;
    typedef uint32_t OPCollection;
    enum OP
    {
        READ = EPOLLIN | EPOLLONESHOT,
        WRITE = EPOLLOUT | EPOLLONESHOT,
        REMOTE_CLOSE = EPOLLRDHUP,
        EXCEPT = EPOLLHUP | EPOLLERR
    };

    enum TYPE
    {
        SERVER_STREAM_SOCKET,
        STREAM_SOCKET,
        DATAGRAM_SOCKET,
    };
};

#endif /* __SELECTABLE_HPP__ */
