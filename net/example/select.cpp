#include <iostream>
#include <thread>
#include <vector>
#include <functional>

#include "net/select/selector.hpp"
#include "net/address.hpp"
#include "net/protocol.hpp"
#include "net/net_exception.hpp"
#include "net/tcp/acceptor.hpp"
#include "net/tcp/socket.hpp"

auto SERVERSOCKETOPS = net::select::Selectable::OP::READ | net::select::Selectable::OP::EXCEPT;
auto SOCKETOPS = net::select::Selectable::OP::READ | net::select::Selectable::OP::REMOTE_CLOSE | net::select::Selectable::OP::EXCEPT;

int main(int argc, char const *argv[])
{
    //构造selector
    using AttachType = std::function<void(net::tcp::Socket &, const net::NetException &)>;
    net::select::Selector<AttachType> selector;

    //构造acceptor
    net::tcp::Acceptor acceptor(net::tcp::ProtocolV4(), net::Address(8888));
    acceptor.bind();
    acceptor.listen();

    //acceptor添加到selector
    selector.add(acceptor.native_handle(), SERVERSOCKETOPS,
                 [&acceptor, &selector](net::tcp::Socket &socket, const net::NetException &error)
                 {
                     if (error.empty())
                     {
                         std::cout << socket.remote_address().to_string() << std::endl;
                     }
                     else
                     {
                         std::cout << error.what() << std::endl;
                     }
                     socket.close();
                 });

    std::thread t([&]()
                  { std::this_thread::sleep_for(std::chrono::seconds(1)); });

    std::vector<net::select::Selected<AttachType>> vec;
    for (;;)
    {
        vec.clear();
        std::size_t size = selector.select(vec, std::chrono::seconds(10));
        for (size_t i = 0; i < size; i++)
        {
            // auto selected = vec[i];
            // auto ops = selected.operation();
            // selected.attachment()();
        }
        break;
    }

    t.join();
    return 0;
}
