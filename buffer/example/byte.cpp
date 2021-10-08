#include <iostream>
#include <vector>

#include "byte/byte.hpp"

int main(int argc, char const *argv[])
{
    byte b;
    b = to_byte(100);
    auto c = b << 1;
    printf("%d\n", to_integer(c));

    std::vector<byte> bytes(10);
    return 0;
}
