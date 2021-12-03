#ifndef __ACCEPTOR_HPP__
#define __ACCEPTOR_HPP__

#include <netinet/in.h>

#include <string.h>
#include <string>

class Selectable;

class Acceptor
{
private:
    struct sockaddr_in _addr;

public:
    Acceptor(const Selectable &selectable);
    ~Acceptor();
    void bind(const std::string &address, int port);

    static Selectable accept(const Selectable &selectable);
    static void bind(const Selectable &selectable, int port);
    static void bind(const Selectable &selectable, const std::string &address, int port);
    static void bind_sstream_socket(const Selectable &selectable, const std::string &address, int port);
    static void bind_sstream_socket(const Selectable &selectable, int port);
    static void bind_datagram_socket(const Selectable &selectable, const std::string &address, int port);
    static void bind_datagram_socket(const Selectable &selectable, int port);
};

#endif /* __ACCEPTOR_HPP__ */
