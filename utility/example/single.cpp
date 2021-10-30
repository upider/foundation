#include <iostream>

#include "utility/singleton.hpp"

class Helper : public Singleton<Helper>
{
public:
    int x = 100;

protected:
    friend class Singleton<Helper>;
    Helper();

public:
    ~Helper();
};

Helper::Helper()
{
    std::cout << "protected contructor" << std::endl;
}

Helper::~Helper()
{
}

int main(int argc, char const *argv[])
{
    Helper::instance().x += 1;
    std::cout << Helper::instance().x << std::endl;
    Helper::instance().x += 1;
    std::cout << Helper::instance().x << std::endl;
    return 0;
}
