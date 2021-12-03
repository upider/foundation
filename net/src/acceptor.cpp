#include <sys/types.h>
#include <sys/socket.h>

#include <stdexcept>

#include "acceptor/acceptor.hpp"
#include "select/selectable.hpp"

Acceptor::Acceptor(const Selectable &selectable)
{
}

Acceptor::~Acceptor()
{
}

void Acceptor::bind(const std::string &address, int port)
{
}

Selectable Acceptor::accept(const Selectable &selectable)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int fd = ::accept(selectable.native_handle(), (struct sockaddr *)&client_addr, &client_addr_len);
    return Selectable(fd, Selectable::Type::STREAM_SOCKET, true);
}

void Acceptor::bind_sstream_socket(const Selectable &selectable, const std::string &address, int port)
{
}

void Acceptor::bind_sstream_socket(const Selectable &selectable, int port)
{
    struct sockaddr_in s_addr;
    ::bzero(&s_addr, sizeof(struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_port = htons(port);
    int ret = -1;
    ret = ::bind(selectable.native_handle(), (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (ret < 0)
    {
        throw std::runtime_error(strerror(errno));
    }
    ret = ::listen(selectable.native_handle(), 128);
    if (ret < 0)
    {
        throw std::runtime_error(strerror(errno));
    }
}

void Acceptor::bind_datagram_socket(const Selectable &selectable, const std::string &address, int port)
{
}

void Acceptor::bind_datagram_socket(const Selectable &selectable, int port)
{
}

void Acceptor::bind(const Selectable &selectable, int port)
{
    if (selectable.type() == Selectable::Type::SERVER_STREAM_SOCKET)
    {
        Acceptor::bind_sstream_socket(selectable, port);
    }
    else if (selectable.type() == Selectable::Type::DATAGRAM_SOCKET)
    {
        Acceptor::bind_datagram_socket(selectable, port);
    }
}

void Acceptor::bind(const Selectable &selectable, const std::string &address, int port)
{
    if (selectable.type() == Selectable::Type::SERVER_STREAM_SOCKET)
    {
        Acceptor::bind_sstream_socket(selectable, address, port);
    }
    else if (selectable.type() == Selectable::Type::DATAGRAM_SOCKET)
    {
        Acceptor::bind_datagram_socket(selectable, address, port);
    }
}