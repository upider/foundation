#include <iostream>

#include "utility/date.hpp"

int main(int argc, char const *argv[])
{
    Date<std::chrono::system_clock> date;
    std::cout << date.format() << std::endl;
    std::cout << date.time_zone() << std::endl;
    return 0;
}
