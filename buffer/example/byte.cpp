#include <iostream>
#include <vector>

#include "buffer/byte.hpp"

int main(int argc, char const *argv[])
{
    Byte b;
    b = to_byte(100);
    auto c = b << 1;
    printf("%d\n", to_integer(c));

    std::vector<Byte> bytes(10);
    return 0;
}
