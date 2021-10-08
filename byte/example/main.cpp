#include <iostream>
#include <vector>

#include "byte.hpp"

int main(int argc, char const *argv[])
{
    byte b;
    int x = to_integer(b);
    b = to_byte(100);
    auto c = b<<1;
    printf("%d\n", c);

    std::vector<byte> bytes(10);
    return 0;
}
