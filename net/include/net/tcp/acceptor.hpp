#ifndef __ACCEPTOR_HPP__
#define __ACCEPTOR_HPP__

#include <functional>

namespace net
{
    class IOExecutor;
    class NetException;
    class Protocol;
    class Address;

    namespace tcp
    {

        class Socket;
        class ProtocolV4;
        class ProtocolV6;
        class Acceptor
        {
        public:
            typedef int native_handle_type;

        private:
            bool _non_blocking{true};
            bool _open{true};
            native_handle_type _native_handle{-1};
            Protocol* _protocol;
            Address* _address;

        public:
            explicit Acceptor(const ProtocolV4& protocol, const Address &addr);
            explicit Acceptor(const ProtocolV6& protocol, const Address &addr);
            ~Acceptor();
            bool operator==(const Acceptor &other);

            const Protocol& protocol() const;
            const Address& local_address() const;
            void close();
            bool is_open();
            bool non_blocking();
            void non_blocking(bool non_block);
            void bind();
            void listen(uint16_t backlog);
            /**
             * @brief 同步accept
             * 
             * @param socket 连接的socket
             */
            void accept(Socket &socket);
            /**
             * @brief 异步accept
             * 
             * @param socket 连接的socket
             * @param executor io线程池
             * @param cb 回调
             */
            void accept(Socket &socket, IOExecutor &executor, std::function<void(const NetException &except)> &&cb);
        };

    } // namespace tcp
} // namespace net
#endif /* __ACCEPTOR_HPP__ */
