#include <iostream>

#include "net/net_exception.hpp"
#include "net/address.hpp"
#include "net/protocol.hpp"
#include "net/io_executor.hpp"
#include "net/tcp/acceptor.hpp"
#include "net/tcp/socket.hpp"

int main(int argc, char const *argv[])
{
    net::IOExecutor executor;
    net::tcp::Acceptor acceptor(net::tcp::ProtocolV4(), net::Address(8888));
    acceptor.bind();
    acceptor.listen();

    acceptor.accept(executor,
                    [](net::tcp::Socket &socket, const net::NetException &err)
                    {
                        if (err.empty())
                        {
                            std::cout << "New Connection" << std::endl;
                            std::cout << socket.remote_address().to_string() << std::endl;
                            std::cout << socket.native_handle() << std::endl;
                            char buf[50];
                            std::this_thread::sleep_for(std::chrono::seconds(3));
                            socket.read(buf, 50);
                            std::cout << "Receive Message: " << buf << std::endl;
                            socket.close();
                        }
                        else
                        {
                            std::cout << "Acceptor Exception" << std::endl;
                        }
                    });

    return 0;
}
