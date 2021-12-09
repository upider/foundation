#ifndef __NET_EXCEPTION_HPP__
#define __NET_EXCEPTION_HPP__

#include <stdexcept>
#include <string>

namespace net
{
    class NetException : public std::runtime_error
    {
    private:
        uint16_t _code{0};

    public:
        NetException(const std::string& message = "");
        NetException(uint16_t code, const std::string& message);
        NetException(uint16_t code);
        ~NetException();

        uint16_t code() const noexcept;
        bool empty() const;
    };

} // namespace net

#endif /* __NET_EXCEPTION_HPP__ */
