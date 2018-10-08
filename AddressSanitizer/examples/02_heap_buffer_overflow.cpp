// ASAN example: heap buffer overflow

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::vector<std::string> data{ "no", "one", "two" };
    std::cout << "You passed " << data[argc - 1] << " args(s)\n";
    return 0;
}
