#include <iostream>

#include "utility/date.hpp"

int main(int argc, char const *argv[])
{
    Date<std::chrono::system_clock> date;
    std::cout << date.time_zone() << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(date.time_point().time_since_epoch()).count() << std::endl;
    std::cout << date.second() << " " << date.milisecond() << " " << date.microsecond() << " " << date.nanosecond() << std::endl;
    std::cout << date.format() << std::endl;

    date+=std::chrono::hours(2);
    std::cout << date.format() << std::endl;

    auto date2 = date + std::chrono::hours(2);
    std::cout << date2.format() << std::endl;
    
    return 0;
}
