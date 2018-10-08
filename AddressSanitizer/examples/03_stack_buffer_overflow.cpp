// ASAN example: stack buffer overflow

#include <array>
#include <iostream>

int main(int argc, char** argv)
{
    std::array<const char*, 3> data{ "no", "one", "two" };
    std::cout << "You passed " << data[argc - 1] << " args(s)\n";
    return 0;
}
