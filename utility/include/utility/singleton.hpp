#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include <type_traits>

template <typename T>
class Singleton
{
protected:
    Singleton();

public:
    virtual ~Singleton();

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    static T &instance();
};

template <typename T>
T &Singleton<T>::instance()
{
    static T inst;
    return inst;
}

template <typename T>
Singleton<T>::Singleton()
{
    static_assert(!std::is_constructible<T>::value, "singleton class's contructor must be inaccessable and Singleton<T> must be it's friend");
}

template <typename T>
Singleton<T>::~Singleton()
{
}

#endif // __SINGLETON_HPP__