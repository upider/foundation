#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include "net/io_task.hpp"

namespace net
{
    class Address;
    class IOExecutor;
    class NetException;
    class Protocol;

    namespace buffer
    {
        class ByteBuffer;
    } // namespace buffer

    namespace tcp
    {
        class ProtocolV4;
        class ProtocolV6;
        class Socket
        {
        public:
            class TCPSocketIOTask : public IOTask
            {
            private:
                friend class Socket;
                Socket *_socket;
                std::function<void(std::size_t bytes, const NetException &except)> _callback;

            public:
                TCPSocketIOTask(Socket *socket, std::function<void(std::size_t bytes, const NetException &except)> &&callback);
                virtual ~TCPSocketIOTask();
                virtual void operator()(Selectable::OPCollection ops);
                virtual Selectable::OPCollection interest();
                virtual Selectable::native_handle_type native_handle();
            };

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
            native_handle_type _native_handle{-1};
            Protocol *_protocol;
            Address *_remote_address;

        public:
            Socket();
            Socket(const Socket &) = delete;
            explicit Socket(const ProtocolV4 &protocol, const Address &remote);
            explicit Socket(const ProtocolV6 &protocol, const Address &remote);
            ~Socket();
            const native_handle_type native_handle() const;
            const Address &remote_address() const;
            const Protocol &protocol() const;
            bool operator==(const Socket &other);

            bool non_blocking();
            void non_blocking(bool non_block);
            //send functions
            int send(const void *data, std::size_t size);
            int send(const buffer::ByteBuffer &buffer);
            int send(const buffer::ByteBuffer *buffer);
            void send(const buffer::ByteBuffer &buffer, IOExecutor &executor, std::function<void(buffer::ByteBuffer &buffer, const NetException &except)> &&cb);
            void send(const void *data, std::size_t size, IOExecutor &executor, std::function<void(std::size_t bytes, const NetException &except)> &&cb);
            //recv functions
            int recv(void *data, std::size_t size);
            int recv(buffer::ByteBuffer &buffer);
            int recv(buffer::ByteBuffer *buffer);
            void recv(buffer::ByteBuffer &buffer, IOExecutor &executor, std::function<void(std::size_t bytes, const NetException &except)> &&cb);
            void recv(void *data, std::size_t size, IOExecutor &executor, std::function<void(std::size_t bytes, const NetException &except)> &&cb);
            void shutdown(int shut_type);
            void close();
            bool is_open();
            int connect();
        };

    } // namespace tcp
} // namespace net

#endif /* __SOCKET_HPP__ */
