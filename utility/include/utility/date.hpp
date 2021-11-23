#ifndef __DATE_HPP__
#define __DATE_HPP__

#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

template <typename Clock>
class Date
{
private:
    std::chrono::time_point<Clock> _time_point;
    bool _local;

private:
    std::tm get_tm(const std::chrono::time_point<Clock> &time_point, bool local);

public:
    Date(const std::chrono::time_point<Clock> &time_point = Clock::now(), bool local = true);
    Date(bool local);
    ~Date();

    int year();
    int month();
    int yday();
    int mday();
    int wday();
    int hour();
    int minute();
    int second();
    int milisecond();
    int microsecond();
    int nanosecond();
    std::string time_zone();

    std::chrono::time_point<Clock> time_point();
    std::string format(const std::string &fmt = "%Y-%m-%d %H:%M:%S");
};

template <typename Clock>
std::tm Date<Clock>::get_tm(const std::chrono::time_point<Clock> &time_point, bool local)
{
    struct tm info;
    time_t now = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch()).count();
    if (local)
    {
        info = *std::localtime(&now);
    }
    else
    {
        info = *std::gmtime(&now);
    }
    return info;
}

template <typename Clock>
Date<Clock>::Date(const std::chrono::time_point<Clock> &time_point, bool local) : _time_point(time_point), _local(local) {}

template <typename Clock>
Date<Clock>::Date(bool local) : _time_point(std::chrono::system_clock::now()), _local(local) {}

template <typename Clock>
Date<Clock>::~Date() {}

template<typename Clock>
int Date<Clock>::year() 
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_year();
}

template<typename Clock>
std::string Date<Clock>::time_zone() 
{
    std::tm tmb = get_tm(_time_point, _local);
    return std::string(tmb.tm_zone);
}

template <typename Clock>
std::chrono::time_point<Clock> Date<Clock>::time_point()
{
    return _time_point;
}

template <typename Clock>
std::string Date<Clock>::format(const std::string &fmt)
{
    std::stringstream ss;
    std::tm tmb = get_tm(_time_point, _local);
    ss << std::put_time(&tmb, fmt.c_str());
    return ss.str();
}

#endif /* __DATE_HPP__ */
