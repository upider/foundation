#include <iostream>
#include <thread>
#include <vector>

#include "select/selector.hpp"
#include "acceptor/acceptor.hpp"

auto SERVERSOCKETOPS = Selectable::Operation::OP_REMOTECLOSE | Selectable::Operation::OP_READ | Selectable::Operation::OP_WRITE | Selectable::Operation::OP_ONESHOT;
auto SOCKETOPS = Selectable::Operation::OP_READ | Selectable::Operation::OP_WRITE | Selectable::Operation::OP_ONESHOT;

int main(int argc, char const *argv[])
{
    std::vector<Selected> vec;
    Selector selector;
    auto server_socket = Selectable::open_server_stream_socket();

    selector.add(server_socket, SERVERSOCKETOPS);
    Acceptor::bind(server_socket, 8888);

    for (;;)
    {
        std::size_t size = selector.select(vec, std::chrono::seconds(100));
        for (size_t i = 0; i < size; i++)
        {
            auto ops = vec[i].operation();
            auto selectable = vec[i].selectable();
            if (ops & Selectable::Operation::OP_EXCEPT)
            {
                std::cout << "exception occured" << std::endl;
                vec[i].release();
            }
            else if (ops & Selectable::Operation::OP_READ)
            {
                std::cout << "can read" << std::endl;
                if (selectable.type() == Selectable::Type::SERVER_STREAM_SOCKET)
                {
                    auto new_socket = Acceptor::accept(selectable);
                    if (new_socket.closed())
                    {
                        continue;
                    }
                    selector.add(new_socket, SOCKETOPS);
                    std::cout << "add new Selectable" << std::endl;
                    vec[i].select(SERVERSOCKETOPS);
                }
                else
                {
                    char buf[50];
                    ::read(selectable.native_handle(), buf, 50);
                    std::cout << "receive message: " << std::string(buf) << std::endl;
                    vec[i].select(SOCKETOPS);
                }
            }
            else if (ops & Selectable::Operation::OP_WRITE)
            {
                std::cout << "can write" << std::endl;
            }
            else if (ops & Selectable::Operation::OP_REMOTECLOSE)
            {
                std::cout << "remote close" << std::endl;
            }
        }
    }

    return 0;
}
