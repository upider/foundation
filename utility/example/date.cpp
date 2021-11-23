#include <iostream>

#include "utility/date.hpp"

int main(int argc, char const *argv[])
{
    Date<std::chrono::system_clock> date;
    std::cout << date.format() << std::endl;
    std::cout << date.time_zone() << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(date.time_point().time_since_epoch()).count() << std::endl;
    std::cout << date.second() << " " << date.milisecond() << " " << date.microsecond() << " " << date.nanosecond() << std::endl;
    return 0;
}
