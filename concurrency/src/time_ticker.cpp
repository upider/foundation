#include <cstddef>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdexcept>

#include "timer/time_ticker.hpp"

TimeTicker::TimeTicker(int seconds, int microSeconds)
    : _timeout(new Timeout{seconds, microSeconds}), _readfds(new FDSet{})
{
    int pip_ret = pipe(_pipefds);
    if (pip_ret)
    {
        throw std::runtime_error("TimeTicker create pipe error.");
    }
}

TimeTicker::~TimeTicker()
{
    if (_timeout != nullptr)
    {
        delete _timeout;
    }
}

bool TimeTicker::tick()
{
    FD_ZERO(_readfds);
    FD_SET(_pipefds[0], _readfds);
    Timeout timeout = *_timeout;
    int ret = select(_pipefds[0] + 1, _readfds, NULL, NULL, &timeout);
    if (ret == -1)
    {
        throw std::runtime_error("TimeTicker select error.");
    }

    //如果FD_ISSET返回值为非0，说明文件描述符就绪
    return (FD_ISSET(_pipefds[0], _readfds)) ? true : false;
}

void TimeTicker::cancel()
{
    write(_pipefds[1], "1", 1);
}