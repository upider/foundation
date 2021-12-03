#include <sys/socket.h>
#include <sys/signalfd.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <cerrno>
#include <stdexcept>

#include "select/selectable.hpp"

Selectable::Selectable(const Selectable &other)
    : _native_handle(other._native_handle), _type(other._type), _non_block(other._non_block), _closed(other._closed) {}

Selectable::Selectable() {}

Selectable::Selectable(const native_handle_type &h, Type t, bool non_block)
    : _native_handle(h), _type(t), _non_block(non_block)
{
    if(h < 0)
    {
        _closed = true;
        return;
    }
    if (non_block)
    {
        this->non_block(non_block);
    }
}

Selectable::~Selectable() {}

const Selectable::Type Selectable::type() const
{
    return _type;
}

const Selectable::native_handle_type Selectable::native_handle() const
{
    return _native_handle;
}

Selectable::OperationCollection Selectable::valid_operations()
{
    switch (_type)
    {
    case SERVER_STREAM_SOCKET:
    {
        return OP_READ | OP_ONESHOT | OP_EXCEPT;
    }
    case STREAM_SOCKET:
    {
        return OP_READ | OP_WRITE | OP_REMOTECLOSE | OP_ONESHOT | OP_EXCEPT;
    }
    case DATAGRAM_SOCKET:
    {
        return OP_READ | OP_WRITE | OP_REMOTECLOSE | OP_ONESHOT | OP_EXCEPT;
    }
    default:
        return 0;
    }
}

bool Selectable::non_block()
{
    return _non_block;
}

void Selectable::non_block(bool non_block)
{
    _non_block = non_block;
    if (non_block)
    {
        fcntl(_native_handle, F_SETFL, fcntl(_native_handle, F_GETFL) | O_NONBLOCK);
    }
    else
    {
        int flags = fcntl(_native_handle, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        fcntl(_native_handle, F_SETFL, flags);
    }
}

void Selectable::close()
{
    if (_closed || _native_handle < 1)
    {
        return;
    }
    _closed = true;
    int res = ::close(_native_handle);
    if (res != 0)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Selectable::shutdown(OperationCollection ops)
{
    if (_native_handle < 1)
    {
        return;
    }
    if ((ops & Operation::OP_READ) && (ops & Operation::OP_WRITE))
    {
        ::shutdown(_native_handle, SHUT_RDWR);
        return;
    }
    else if (ops & Operation::OP_READ)
    {
        ::shutdown(_native_handle, SHUT_RD);
    }
    else if (ops & Operation::OP_WRITE)
    {
        ::shutdown(_native_handle, SHUT_WR);
    }
}

bool Selectable::operator==(const Selectable &selectable)
{
    return (_type == selectable._type) && (_native_handle == selectable._native_handle);
}

bool Selectable::closed() 
{
    return _closed;    
}

Selectable Selectable::open_stream_socket(bool non_block)
{
    int fd = -1;
    fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    return Selectable(fd, STREAM_SOCKET, non_block);
}

Selectable Selectable::open_server_stream_socket(bool non_block)
{
    int fd = -1;
    if ((fd = ::socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw std::runtime_error(strerror(errno));
    }

    return Selectable(fd, SERVER_STREAM_SOCKET, non_block);
}

Selectable Selectable::open_datagram_socket(bool non_block)
{
    int fd = -1;
    fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    return Selectable(fd, DATAGRAM_SOCKET, non_block);
}