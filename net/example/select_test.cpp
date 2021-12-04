#include <iostream>
#include <thread>
#include <vector>

#include "select/selector.hpp"
#include "address/address.hpp"

auto SERVERSOCKETOPS = EPOLLONESHOT | Selectable::OP::READ | Selectable::OP::EXCEPT;
auto SOCKETOPS = EPOLLONESHOT | Selectable::OP::READ | Selectable::OP::REMOTE_CLOSE | Selectable::OP::EXCEPT;

int main(int argc, char const *argv[])
{
    std::vector<Selected<int>> vec;
    Selector<int> selector;
    auto server_socket = Socket::stream_socket();

    //bind 应该在 add之前
    server_socket.bind("192.168.37.204", 8888);
    selector.add(server_socket, SERVERSOCKETOPS);

    std::thread t([&]()
                  {
                      std::this_thread::sleep_for(std::chrono::seconds(1));
                      //   server_socket.shutdown(Selectable::OP::OP_READ);
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
            if (ops & Selectable::OP::EXCEPT)
            {
                std::cout << "exception occured" << std::endl;
                std::cout << "Selectable == " << selectable.native_handle() << std::endl;
                std::cout << "ops == " << ops << std::endl;
                vec[i].release();
                if (selectable.acceptable())
                {
                    std::cout << "SERVER_STREAM_SOCKET close, return" << std::endl;
                    return 0;
                }
            }
            else if (ops & Selectable::OP::REMOTE_CLOSE)
            {
                std::cout << "remote close, " << selectable.remote_addr().to_string() << std::endl;
                vec[i].release();
            }
            else if (ops & Selectable::OP::READ)
            {
                if (selectable.acceptable())
                {
                    std::cout << "can accept" << std::endl;
                    auto new_socket = selectable.accept();
                    if (!new_socket.closed())
                    {
                        selector.add(new_socket, SOCKETOPS);
                        std::cout << "remote connect, " << new_socket.remote_addr().to_string() << std::endl;
                    }
                    vec[i].select(SERVERSOCKETOPS);
                }
                else
                {
                    std::cout << "can read" << std::endl;
                    char buf[50]{};
                    selectable.read(buf, 50);
                    std::cout << "receive message: " << buf << std::endl;
                    vec[i].select(SOCKETOPS);
                }
            }
            else if (ops & Selectable::OP::WRITE)
            {
                std::cout << "can write" << std::endl;
            }
            else
            {
                std::cout << "unknown event" << std::endl;
                std::cout << "ops == " << ops << std::endl;
            }
        }
    }

    t.join();
    return 0;
}
