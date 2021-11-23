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
    std::tm get_tm(const std::chrono::time_point<Clock> &time_point, bool local) const;

public:
    Date(const std::chrono::time_point<Clock> &time_point = Clock::now(), bool local = true);
    Date(bool local);
    Date(const Date &other);
    ~Date();

    std::size_t year() const;
    std::size_t month() const;
    std::size_t yday() const;
    std::size_t mday() const;
    std::size_t wday() const;
    std::size_t hour() const;
    std::size_t minute() const;
    std::size_t second() const;
    std::size_t milisecond() const;
    std::size_t microsecond() const;
    std::size_t nanosecond() const;
    std::string time_zone() const;

    Date &operator=(const Date &other);

    template <typename Rep, typename Period>
    Date operator+(const std::chrono::duration<Rep, Period> &duration);
    template <typename Rep, typename Period>
    Date operator-(const std::chrono::duration<Rep, Period> &duration);
    template <typename Rep, typename Period>
    Date &operator+=(const std::chrono::duration<Rep, Period> &duration);
    template <typename Rep, typename Period>
    Date &operator-=(const std::chrono::duration<Rep, Period> &duration);

    std::chrono::time_point<Clock> time_point() const;
    std::string format(const std::string &fmt = "%Y-%m-%d %H:%M:%S") const;
};

template <typename Clock>
std::tm Date<Clock>::get_tm(const std::chrono::time_point<Clock> &time_point, bool local) const
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
Date<Clock>::Date(const Date &other) : _time_point(other._time_point), _local(other._local) {}

template <typename Clock>
Date<Clock>::~Date() {}

template <typename Clock>
std::size_t Date<Clock>::year() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_year();
}

template <typename Clock>
std::size_t Date<Clock>::month() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_mon();
}

template <typename Clock>
std::size_t Date<Clock>::yday() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_yday();
}

template <typename Clock>
std::size_t Date<Clock>::mday() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_mday();
}

template <typename Clock>
std::size_t Date<Clock>::wday() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_wday();
}

template <typename Clock>
std::size_t Date<Clock>::hour() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_hour();
}

template <typename Clock>
std::size_t Date<Clock>::minute() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_min();
}

template <typename Clock>
std::size_t Date<Clock>::second() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return tmb.tm_sec;
}

template <typename Clock>
std::size_t Date<Clock>::milisecond() const
{
    auto milisec = std::chrono::duration_cast<std::chrono::milliseconds>(_time_point.time_since_epoch()).count();
    return milisec % 1000;
}

template <typename Clock>
std::size_t Date<Clock>::microsecond() const
{
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(_time_point.time_since_epoch()).count();
    return microsec % (1000 * 1000);
}

template <typename Clock>
std::size_t Date<Clock>::nanosecond() const
{
    auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(_time_point.time_since_epoch()).count();
    return nanosec % (1000 * 1000 * 1000);
}

template <typename Clock>
std::string Date<Clock>::time_zone() const
{
    std::tm tmb = get_tm(_time_point, _local);
    return std::string(tmb.tm_zone);
}

template <typename Clock>
Date<Clock> &Date<Clock>::operator=(const Date &other)
{
    _time_point = other._time_point;
    _local = other._local;
    return *this;
}

template <typename Clock>
template <typename Rep, typename Period>
Date<Clock> Date<Clock>::operator-(const std::chrono::duration<Rep, Period> &duration)
{
    auto t = _time_point - duration;
    Date date(t);
    return date;
}

template <typename Clock>
template <typename Rep, typename Period>
Date<Clock> &Date<Clock>::operator+=(const std::chrono::duration<Rep, Period> &duration)
{
    _time_point += duration;
    return *this;
}

template <typename Clock>
template <typename Rep, typename Period>
Date<Clock> &Date<Clock>::operator-=(const std::chrono::duration<Rep, Period> &duration)
{
    _time_point -= duration;
    return *this;
}

template <typename Clock>
template <typename Rep, typename Period>
Date<Clock> Date<Clock>::operator+(const std::chrono::duration<Rep, Period> &duration)
{
    auto t = _time_point + duration;
    Date date(t);
    return date;
}

template <typename Clock>
std::chrono::time_point<Clock> Date<Clock>::time_point() const
{
    return _time_point;
}

template <typename Clock>
std::string Date<Clock>::format(const std::string &fmt) const
{
    std::stringstream ss;
    std::tm tmb = get_tm(_time_point, _local);
    ss << std::put_time(&tmb, fmt.c_str());
    return ss.str();
}

#endif /* __DATE_HPP__ */
