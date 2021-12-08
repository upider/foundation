#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <cstring>
#include <cassert>
#include <cerrno>
#include <stdexcept>

#include "net/posix.hpp"
#include "net/address.hpp"
#include "net/io_executor.hpp"
#include "net/net_exception.hpp"
#include "net/protocol.hpp"
#include "net/tcp/acceptor.hpp"
#include "net/tcp/socket.hpp"

namespace net
{
    namespace tcp
    {
        Acceptor::Acceptor(const ProtocolV4 &protocol, const Address &addr) : _protocol(new ProtocolV4(protocol)), _address(new Address(addr))
        {
            _native_handle = ::socket(_protocol->family(), _protocol->type(), _protocol->protocol());
            assert(_native_handle != -1);
            non_blocking(true);
        }

        Acceptor::Acceptor(const ProtocolV6 &protocol, const Address &addr) : _protocol(new ProtocolV6(protocol)), _address(new Address(addr))
        {
            _native_handle = ::socket(_protocol->family(), _protocol->type(), _protocol->protocol());
            assert(_native_handle != -1);
            non_blocking(true);
        }

        Acceptor::~Acceptor()
        {
            delete _protocol;
            delete _address;
        }

        bool Acceptor::operator==(const Acceptor &other)
        {
            return bool(_native_handle == other._native_handle);
        }

        const Protocol& Acceptor::protocol() const
        {
            return *_protocol;    
        }
        
        const Address &Acceptor::local_address() const
        {
            return *_address;
        }

        void Acceptor::close()
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

        bool Acceptor::is_open()
        {
            return _open;
        }

        bool Acceptor::non_blocking()
        {
            return _non_blocking;
        }

        void Acceptor::non_blocking(bool non_block)
        {
            _non_blocking = non_block;
            Posix::non_blocking(_native_handle, non_block);
        }

        void Acceptor::bind()
        {
            auto s_addr = Posix::sock_address(_address->ip(), _address->port(), _protocol->family());

            int ret = -1;
            ret = ::bind(_native_handle, (struct sockaddr *)&s_addr, sizeof(s_addr));
            if (ret < 0)
            {
                throw NetException(strerror(errno));
            }
        }

        void Acceptor::listen(uint16_t backlog)
        {
            int ret = -1;
            ret = ::listen(_native_handle, backlog);
            if (ret < 0)
            {
                throw NetException(strerror(errno));
            }
        }

        void Acceptor::accept(Socket &socket)
        {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int fd = ::accept(_native_handle, (struct sockaddr *)&client_addr, &client_addr_len);
            Address socket_address = Posix::address(client_addr);
            socket._native_handle = fd;
            socket._remote_address = new Address(socket_address);
            socket.non_blocking(true);
            return;
        }

        void Acceptor::accept(Socket &socket, IOExecutor &executor, std::function<void(const NetException &except)> &&cb)
        {
        }

    } // namespace tcp

} // namespace net