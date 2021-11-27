#include <iostream>
#include <vector>

#include "byte/byte.hpp"
#include "buffer/byte_buffer.hpp"

int main(int argc, char const *argv[])
{
    std::vector<int> a;
    Byte bytes[50];
    std::memcpy(bytes, "xxxxxxx", 8);
    ByteBuffer bb(100);
    bb.reset(bytes, 8, 50);
    std::cout << "writable bytes = " << bb.writable() << std::endl;
    bb[4] = to_byte('c');
    bb.write<int>(100, 0);
    bb.write<double>(3, 4);
    std::cout << bb.str() << std::endl;
    std::cout << bb.read<int>(0) << std::endl;
    std::cout << bb.read<int>() << std::endl;
    std::cout << bb.read<int>() << std::endl;
    return 0;
}
