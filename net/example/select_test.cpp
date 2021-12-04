#include <iostream>
#include <thread>
#include <vector>

#include "select/selector.hpp"
#include "acceptor/acceptor.hpp"

auto SERVERSOCKETOPS = Selectable::Operation::READ | Selectable::Operation::EXCEPT;
auto SOCKETOPS = Selectable::Operation::READ | Selectable::Operation::REMOTE_CLOSE | Selectable::Operation::EXCEPT;

int main(int argc, char const *argv[])
{
    std::vector<Selected> vec;
    Selector selector;
    auto server_socket = Selectable::open_sstream_socket();

    selector.add(server_socket, SERVERSOCKETOPS);
    Acceptor::bind(server_socket, 8888);

    std::thread t([&]()
                  {
                      std::this_thread::sleep_for(std::chrono::seconds(1));
                      //   server_socket.shutdown(Selectable::Operation::OP_READ);
                      //   server_socket.close();
                  });

    for (;;)
    {
        vec.clear();
        std::size_t size = selector.select(vec, std::chrono::seconds(10));
        for (size_t i = 0; i < size; i++)
        {
            auto ops = vec[i].operation();
            auto selectable = vec[i].selectable();
            if (ops & Selectable::Operation::EXCEPT)
            {
                std::cout << "exception occured" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
                std::cout << "ops == " << ops << std::endl;
                vec[i].release();
                if(selectable.type() == Selectable::Type::SERVER_STREAM_SOCKET)
                {
                    std::cout << "SERVER_STREAM_SOCKET close, return" << std::endl;
                    return 0;
                }
            }
            else if (ops & Selectable::Operation::REMOTE_CLOSE)
            {
                std::cout << "remote close" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
                vec[i].release();
            }
            else if (ops & Selectable::Operation::READ)
            {
                std::cout << "can read" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
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
                    char buf[50]{};
                    ::recv(selectable.native_handle(), buf, 50, MSG_DONTWAIT);
                    std::cout << "receive message: " << buf << std::endl;
                    vec[i].select(SOCKETOPS);
                }
            }
            else if (ops & Selectable::Operation::WRITE)
            {
                std::cout << "can write" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
            }
            else
            {
                std::cout << "unknown event" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
                std::cout << "ops == " << ops << std::endl;
            }
        }
    }

    t.join();
    return 0;
}
