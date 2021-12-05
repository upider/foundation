#ifndef __ADDRESS_HPP__
#define __ADDRESS_HPP__

#include <string>

class Address
{
private:
    std::string _ip;
    int _port;

public:
    Address(const std::string& ip, int port);
    Address();
    ~Address();

    std::string to_string();
    std::string ip();
    int port();
    void ip(const std::string& ip);
    void port(int port);
};

#endif /* __ADDRESS_HPP__ */
