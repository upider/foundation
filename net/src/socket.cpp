#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring>
#include <cassert>
#include <cerrno>
#include <stdexcept>

#include "net/address.hpp"
#include "net/protocol.hpp"
#include "net/posix.hpp"
#include "net/net_exception.hpp"
#include "net/tcp/socket.hpp"

namespace net
{
    namespace tcp
    {
        Socket::Socket(const ProtocolV4 &protocol, const Address &remote) : _protocol(new ProtocolV4(protocol)), _remote_address(new Address(remote))
        {
            _native_handle = ::socket(_protocol->family(), _protocol->type(), _protocol->protocol());
            assert(_native_handle != -1);
            non_blocking(true);
        }

        Socket::Socket(const ProtocolV6 &protocol, const Address &remote) : _protocol(new ProtocolV6(protocol)), _remote_address(new Address(remote))
        {
            _native_handle = ::socket(_protocol->family(), _protocol->type(), _protocol->protocol());
            assert(_native_handle != -1);
            non_blocking(true);
        }

        Socket::Socket() {}
        Socket::~Socket() { delete _remote_address; }

        const Socket::native_handle_type Socket::native_handle() const
        {
            return _native_handle;
        }

        const Protocol &Socket::protocol() const
        {
            return *_protocol;
        }

        const Address &Socket::remote_address() const
        {

            return *_remote_address;
        }

        bool Socket::non_blocking()
        {
            return _non_blocking;
        }

        void Socket::non_blocking(bool non_block)
        {
            _non_blocking = non_block;
            Posix::non_blocking(_native_handle, non_block);
        }

        int Socket::read(void *data, std::size_t size)
        {
            return ::recv(_native_handle, data, size, MSG_DONTWAIT);
        }

        int Socket::write(void *data, std::size_t size)
        {
            return ::send(_native_handle, data, size, MSG_DONTWAIT);
        }

        void Socket::read(void *data, std::size_t size, IOExecutor& executor, std::function<void(const NetException&, std::size_t bytes)>&& cb) 
        {
        }
        
        void Socket::write(void *data, std::size_t size, IOExecutor& executor, std::function<void(const NetException&, std::size_t bytes)>&& cb) 
        {
        }
        
        void Socket::shutdown(int shut_type)
        {
            if (_native_handle < 1 || shut_type < SHUT_RD || shut_type > SHUT_RDWR)
            {
                return;
            }
            ::shutdown(_native_handle, shut_type);
        }

        void Socket::close()
        {
            if (!_open || _native_handle < 1)
            {
                return;
            }
            _open = false;
            int res = ::close(_native_handle);
            if (res != 0)
            {
                throw NetException(strerror(errno));
            }
        }

        bool Socket::operator==(const Socket &other)
        {
            return bool(_native_handle == other._native_handle);
        }

        int Socket::connect()
        {
            auto ip = _remote_address->ip();
            auto port = _remote_address->port();
            struct sockaddr_in s_addr = Posix::sock_address(ip, port, _protocol->family());
            if (ip.empty())
            {
                s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            }
            else
            {
                s_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            }

            int ret = ::connect(_native_handle, (struct sockaddr *)&s_addr, sizeof(s_addr));
            if (ret == 0)
            {
            }
            return ret;
        }

    } // namespace tcp
} // namespace net