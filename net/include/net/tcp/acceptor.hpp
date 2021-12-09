#ifndef __ACCEPTOR_HPP__
#define __ACCEPTOR_HPP__

#include <functional>
#include <memory>

#include "net/io_task.hpp"

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
            class AcceptorIOTask : public IOTask
            {
            private:
                friend class Acceptor;
                Acceptor *_acceptor;
                std::function<void(Socket &, const NetException &)> _callback;

            public:
                AcceptorIOTask(Acceptor *acceptor, std::function<void(Socket &, const NetException &except)>&& callback);
                virtual ~AcceptorIOTask();
                virtual void operator()(Selectable::OPCollection ops);
                virtual Selectable::OPCollection interest();
                virtual Selectable::native_handle_type native_handle();
            };

            bool _non_blocking{true};
            bool _open{true};
            native_handle_type _native_handle{-1};
            Protocol *_protocol;
            Address *_address;

        public:
            explicit Acceptor(const ProtocolV4 &protocol, const Address &addr);
            explicit Acceptor(const ProtocolV6 &protocol, const Address &addr);
            ~Acceptor();
            Acceptor(const Acceptor &) = delete;

            native_handle_type native_handle();
            const Protocol &protocol() const;
            const Address &local_address() const;
            void close();
            bool is_open();
            bool non_blocking();
            void non_blocking(bool non_block);
            void bind();
            void listen(uint16_t backlog = 128);
            /**
             * @brief 同步accept
             * 
             * @param socket 连接的socket
             */
            void accept(Socket &socket);
            /**
             * @brief 异步accept, io发生异常时Acceptor会被从IOExecutor中删除
             *        Acceptor在等待事件发生时不能析构
             * 
             * @param socket 连接的socket
             * @param executor io线程池
             * @param cb 回调
             */
            void accept(IOExecutor &executor, std::function<void(Socket &, const NetException &)> &&cb);
        };

    } // namespace tcp
} // namespace net
#endif /* __ACCEPTOR_HPP__ */
