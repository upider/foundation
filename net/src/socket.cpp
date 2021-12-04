#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring>
#include <cerrno>
#include <stdexcept>

#include "address/address.hpp"
#include "socket/socket.hpp"

Socket::Socket() {}

Socket::~Socket() {}

const Socket::native_handle_type Socket::native_handle() const
{
    return _native_handle;
}

Address Socket::remote_addr()
{
    Address address;
    sockaddr_storage storage;
    socklen_t sock_len = sizeof(storage); // 必须给初值
    int ret = ::getpeername(_native_handle, (sockaddr *)&storage, &sock_len);
    if (ret == 0)
    {
        if (storage.ss_family == AF_INET)
        {
            sockaddr_in *addr = (sockaddr_in *)&storage;
            char ip[INET_ADDRSTRLEN];
            ::inet_ntop(AF_INET, &addr->sin_addr, ip, sock_len);
            address.ip(ip);
            address.port(ntohs(addr->sin_port));
        }
        else if (storage.ss_family == AF_INET6)
        {
            sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
            char ip[INET6_ADDRSTRLEN];
            ::inet_ntop(AF_INET6, &addr->sin6_addr, ip, sock_len);
            address.ip(ip);
            address.port(ntohs(addr->sin6_port));
        }
    }
    return address;
}

Address Socket::local_addr()
{
    Address address;
    sockaddr_storage storage;
    socklen_t addr_len = sizeof(storage);
    /* 获取本端的socket地址 */
    int ret = ::getsockname(_native_handle, (struct sockaddr *)&storage, &addr_len);
    if (ret == 0)
    {
        if (storage.ss_family == AF_INET)
        {
            sockaddr_in *addr = (sockaddr_in *)&storage;
            address.ip(inet_ntoa(addr->sin_addr));
            address.port(ntohs(addr->sin_port));
        }
        if (storage.ss_family == AF_INET6)
        {
            sockaddr_in6 *addr = (sockaddr_in6 *)&storage;
            char ip[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(addr));
            address.ip(ip);
            address.port(ntohs(addr->sin6_port));
        }
    }
    return address;
}

bool Socket::non_blocking()
{
    return _non_blocking;
}

void Socket::non_blocking(bool non_block)
{
    _non_blocking = non_block;
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

int Socket::read(void *data, std::size_t size)
{
    return ::recv(_native_handle, data, size, MSG_DONTWAIT);
}

int Socket::write(void *data, std::size_t size)
{
    return ::send(_native_handle, data, size, MSG_DONTWAIT);
}

void Socket::shutdown(Selectable::OPCollection ops)
{
    if (_native_handle < 1)
    {
        return;
    }
    if ((ops & Selectable::OP::READ) && (ops & Selectable::OP::WRITE))
    {
        ::shutdown(_native_handle, SHUT_RDWR);
        return;
    }
    else if (ops & Selectable::OP::READ)
    {
        ::shutdown(_native_handle, SHUT_RD);
    }
    else if (ops & Selectable::OP::WRITE)
    {
        ::shutdown(_native_handle, SHUT_WR);
    }
}

void Socket::close()
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

bool Socket::closed()
{
    return _closed;
}

Selectable::OPCollection Socket::valid_ops()
{
    switch (_type)
    {
    case Selectable::TYPE::SERVER_STREAM_SOCKET:
    {
        return Selectable::OP::READ | Selectable::OP::WRITE | Selectable::OP::EXCEPT;
    }
    case Selectable::TYPE::STREAM_SOCKET:
    {
        return Selectable::OP::READ | Selectable::OP::WRITE | Selectable::OP::REMOTE_CLOSE | Selectable::OP::EXCEPT;
    }
    case Selectable::TYPE::DATAGRAM_SOCKET:
    {
        return Selectable::OP::READ | Selectable::OP::WRITE | Selectable::OP::EXCEPT;
    }
    default:
        return 0;
    }
}

bool Socket::operator==(const Socket &other)
{
    return bool(_native_handle == other._native_handle);
}

bool Socket::acceptable()
{
    return bool(_type == Selectable::TYPE::SERVER_STREAM_SOCKET);
}

Socket Socket::accept()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int fd = ::accept(_native_handle, (struct sockaddr *)&client_addr, &client_addr_len);
    Socket socket;
    socket._native_handle = fd;
    socket._type = Selectable::TYPE::STREAM_SOCKET;
    socket.non_blocking(true);
    return socket;
}

void Socket::bind(const std::string &ip, int port, int backlog)
{
    _type = Selectable::TYPE::SERVER_STREAM_SOCKET;
    struct sockaddr_in s_addr;
    ::bzero(&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    if (ip.empty())
    {
        s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        s_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }

    int ret = -1;
    ret = ::bind(_native_handle, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret < 0)
    {
        throw std::runtime_error(strerror(errno));
    }
    ret = ::listen(_native_handle, backlog);
    if (ret < 0)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Socket::bind(int port, int backlog)
{
    bind("", port, backlog);
}

Socket Socket::stream_socket()
{
    int fd = -1;
    fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    Socket socket;
    socket._native_handle = fd;
    socket._type = Selectable::TYPE::STREAM_SOCKET;
    socket.non_blocking(true);
    return socket;
}

Socket Socket::datagram_socket()
{
    int fd = -1;
    fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        throw std::runtime_error(strerror(errno));
    }
    Socket socket;
    socket._native_handle = fd;
    socket._type = Selectable::TYPE::DATAGRAM_SOCKET;
    socket.non_blocking(true);
    return socket;
}