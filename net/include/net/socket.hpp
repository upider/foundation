#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>

#include "select/selectable.hpp"

class Address;

class Socket
{
public:
    typedef Selectable::native_handle_type native_handle_type;

private:
    Socket();
    bool _acceptable{false};
    bool _non_blocking{true};
    bool _closed{false};
    Selectable::TYPE _type;
    native_handle_type _native_handle;

public:
    ~Socket();
    const native_handle_type native_handle() const;

    Address remote_addr();
    Address local_addr();
    bool non_blocking();
    void non_blocking(bool non_block);
    int read(void* data, std::size_t size);
    int write(void* data, std::size_t size);
    void shutdown(Selectable::OPCollection ops);
    void close();
    bool closed();
    Selectable::OPCollection valid_ops();
    bool operator==(const Socket& other);

    bool acceptable();
    Socket accept();
    void bind(const std::string& ip, int port, int backlog = 128);
    void bind(int port, int backlog = 128);

    static Socket stream_socket();
    static Socket datagram_socket();
};

#endif /* __SOCKET_HPP__ */
