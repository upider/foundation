#include <iostream>

#include "utility/env.hpp"

int main(int argc, char const *argv[])
{
    std::cout << std::boolalpha <<  get_env<bool>("testbool") << std::endl;
    return 0;
}
