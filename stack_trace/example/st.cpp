#include <sys/signal.h>

#include <iostream>
#include <sys/signal.h>
#include <thread>

#include "stack_trace/stack_trace.hpp"

void handler(int)
{
    std::cout << stacktrace_str(256) << std::endl;
    std::exit(-1);
}

int main(int argc, char const *argv[])
{
    signal(SIGSEGV, handler);
    signal(SIGFPE, handler);
    auto x = 100 / 0;

    return 0;
}
