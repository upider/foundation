#ifndef __ADDRESS_HPP__
#define __ADDRESS_HPP__

#include <string>

namespace net
{
    class Address
    {
    protected:
        std::string _ip;
        uint16_t _port;

    public:
        Address() = default;
        Address(const std::string &ip, uint16_t port);
        Address(uint16_t port);
        virtual ~Address();

        virtual std::string to_string() const;
        virtual std::string ip();
        virtual uint16_t port();
        virtual void ip(const std::string &ip);
        virtual void port(uint16_t port);
    };
    
} // namespace net

#endif /* __ADDRESS_HPP__ */
