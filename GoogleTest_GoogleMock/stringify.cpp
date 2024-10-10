#include "stringify.hpp"

std::string Stringify::to_string(int value)
{
    bool neg = value < 0;
    if (neg) {
        value = abs(value);
    }

    std::string s;
    do
    {
        int8_t remainder = value % 10;
        value /= 10;
        s.insert(0, 1, '0' + remainder);
    } while (value > 0);

    if (neg) {
        s.insert(0, 1, '-');
    }
    return s;
}
