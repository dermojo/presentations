// UBSAN example: invalid enum
#include <cstdlib>
#include <iostream>

// note: only works with Clang, but not with enum classes...
enum Color
{
    red = 0,
    green = 1,
    blue = 2,
};

inline std::ostream& operator<<(std::ostream& os, Color c)
{
    switch (c)
    {
    case Color::red:
        return os << "red";
    case Color::green:
        return os << "green";
    case Color::blue:
        return os << "blue";
    default:
        return os << "???";
    }
}

int main(int argc, const char** argv)
{
    if (argc < 2)
        return 1;
    Color c = static_cast<Color>(atoi(argv[1]));
    std::cout << c << "\n";
    return 0;
}
