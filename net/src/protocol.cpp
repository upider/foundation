#include <sys/socket.h>

#include "net/protocol.hpp"

namespace net
{
    Protocol::Protocol() {}
    Protocol::~Protocol() {}

    namespace tcp
    {
        ProtocolV4::ProtocolV4() {}
        ProtocolV4::~ProtocolV4() {}
        ProtocolV6::ProtocolV6() {}
        ProtocolV6::~ProtocolV6() {}

        int ProtocolV4::family() const
        {
            return AF_INET;
        }

        int ProtocolV4::type() const
        {
            return SOCK_STREAM;
        }

        int ProtocolV4::protocol() const
        {
            return 0;
        }

        int ProtocolV6::family() const
        {
            return AF_INET6;
        }

        int ProtocolV6::type() const
        {
            return SOCK_STREAM;
        }

        int ProtocolV6::protocol() const
        {
            return 0;
        }
    } // namespace tcp

    namespace udp
    {
        ProtocolV4::ProtocolV4() {}
        ProtocolV4::~ProtocolV4() {}
        ProtocolV6::ProtocolV6() {}
        ProtocolV6::~ProtocolV6() {}

        int ProtocolV4::family() const
        {
            return AF_INET;
        }

        int ProtocolV4::type() const
        {
            return SOCK_DGRAM;
        }

        int ProtocolV4::protocol() const
        {
            return 0;
        }

        int ProtocolV6::family() const
        {
            return AF_INET6;
        }

        int ProtocolV6::type() const
        {
            return SOCK_DGRAM;
        }

        int ProtocolV6::protocol() const
        {
            return 0;
        }
    } // namespace udp

} // namespace net
