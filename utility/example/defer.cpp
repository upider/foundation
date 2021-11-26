#include <iostream>
#include <string>
#include <thread>

#include "utility/defer.hpp"

int f1()
{
    int x = 0;
    std::string str;
    //无法改变函数返回值
    defer([&x, &str]() { ++x; });
    return x;
}

class A
{
public:
    A() {}
    ~A()
    {
        std::cout << "/* message */" << std::endl;
    }
};

int main(int argc, char const *argv[])
{
    int x = 100;
    defer(
        [&]()
        {
            std::cout << "++x == " << ++x << std::endl;
        });
    defer(
        []()
        {
            std::cout << "/* message */" << std::endl;
        });

    std::cout << "f1() = " << f1() << std::endl;

    return 0;
}
