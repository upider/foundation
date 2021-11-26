#include <iostream>

#include "byte/byte.hpp"
#include "buffer/byte_buffer.hpp"

int main(int argc, char const *argv[])
{
    Byte bytes[50];
    std::memcpy(bytes, "xxxxxxx", 8);
    ByteBuffer<Byte> bb(100);
    bb.reset(bytes, 8, 50);
    std::cout << "writable bytes = " << bb.writable() << std::endl;
    std::cout << bb.str() << std::endl;
    return 0;
}
