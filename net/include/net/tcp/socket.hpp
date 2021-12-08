#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <functional>

namespace net
{
    class Address;
    class IOExecutor;
    class NetException;
    class Protocol;
    namespace tcp
    {
        class ProtocolV4;
        class ProtocolV6;
        class Socket
        {
        public:
            friend class Acceptor;
            enum
            {
                SHUT_RD = 0, /* No more receptions.  */
                SHUT_WR,     /* No more transmissions.  */
                SHUT_RDWR    /* No more receptions or transmissions.  */
            };
            typedef int native_handle_type;

        private:
            bool _non_blocking{true};
            bool _open{true};
            native_handle_type _native_handle;
            Protocol* _protocol;
            Address *_remote_address;

        public:
            Socket();
            explicit Socket(const ProtocolV4& protocol, const Address &remote);
            explicit Socket(const ProtocolV6& protocol, const Address &remote);
            ~Socket();
            const native_handle_type native_handle() const;
            const Address &remote_address() const;
            const Protocol& protocol() const;
            bool operator==(const Socket &other);

            bool non_blocking();
            void non_blocking(bool non_block);
            int read(void *data, std::size_t size);
            int write(void *data, std::size_t size);
            void read(void *data, std::size_t size, IOExecutor& executor, std::function<void(const NetException&, std::size_t bytes)>&& cb);
            void write(void *data, std::size_t size, IOExecutor& executor, std::function<void(const NetException&, std::size_t bytes)>&& cb);
            void shutdown(int shut_type);
            void close();
            bool is_open();
            int connect();
        };

    } // namespace tcp
} // namespace net

#endif /* __SOCKET_HPP__ */
