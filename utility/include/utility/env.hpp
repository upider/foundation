#ifndef __ENV_HPP__
#define __ENV_HPP__

#include <stdlib.h>

#include <string>

template <typename T>
T get_env(const std::string &name)
{
}

/**********字符串*************/

std::string get_env(const std::string &name)
{
    char *env = ::getenv(name.c_str());
    if (env == NULL)
    {
        std::string value;
        return std::move(value);
    }
    else
    {
        std::string value(::getenv(name.c_str()));
        return std::move(value);
    }
}

/**********数字*************/
template <>
int get_env<int>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stoi(value);
}

template <>
long get_env<long>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stol(value);
}

template <>
double get_env<double>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stod(value);
}

template <>
float get_env<float>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stof(value);
}

template <>
long long get_env<long long>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stoll(value);
}

template <>
long double get_env<long double>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stold(value);
}

template <>
unsigned long get_env<unsigned long>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stoul(value);
}

template <>
unsigned long long get_env<unsigned long long>(const std::string &name)
{
    std::string value = get_env(name);
    return std::stoull(value);
}

/**********bool*************/
template <>
bool get_env<bool>(const std::string &name)
{
    std::string value = get_env(name);
    if (value == "True" || value == "true")
    {
        return true;
    }
    else if (value == "False" || value == "false")
    {
        return false;
    }
    else if (std::stoi(value) > 0)
    {
        return true;
    }
    else if (std::stoi(value) <= 0)
    {
        return false;
    }
    else
    {
        return false;
    }
}

#endif // __ENV_HPP__