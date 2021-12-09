#include "net/protocol.hpp"
#include "net/address.hpp"

namespace net
{
    Address::Address(const std::string &ip, uint16_t port) : _ip(ip), _port(port) {}
    Address::Address(uint16_t port) : _port(port) {}
    Address::~Address() {}

    std::string Address::to_string() const
    {
        std::string str;
        str.append(_ip).append(":").append(std::to_string(_port));
        return str;
    }

    std::string Address::ip()
    {
        return _ip;
    }

    uint16_t Address::port()
    {
        return _port;
    }

    void Address::ip(const std::string &ip)
    {
        _ip = ip;
    }

    void Address::port(uint16_t port)
    {
        _port = port;
    }
} // namespace net