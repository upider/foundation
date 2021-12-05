#include "net/address.hpp"

Address::Address(const std::string &ip, int port)
    : _ip(ip), _port(port) {}

Address::Address() {}
Address::~Address() {}

std::string Address::to_string()
{
    std::string str;
    str.append(_ip).append(":").append(std::to_string(_port));
    return str;
}

std::string Address::ip()
{
    return _ip;
}

int Address::port()
{
    return _port;
}

void Address::ip(const std::string &ip)
{
    _ip = ip;
}

void Address::port(int port)
{
    _port = port;
}