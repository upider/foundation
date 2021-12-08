#include "net/net_exception.hpp"

namespace net
{
    NetException::NetException(const std::string &message) : std::runtime_error(message) {}
    NetException::NetException(uint16_t code, const std::string &message) : std::runtime_error(message), _code(code) {}
    NetException::NetException(uint16_t code) : std::runtime_error("unknown"), _code(code) {}
    NetException::~NetException() {}

    uint16_t NetException::code() const noexcept
    {
        return _code;
    }

    bool NetException::empty() 
    {
        return (_code == 0 && what() == "");
    }
} // namespace net