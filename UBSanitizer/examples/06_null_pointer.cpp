// UBSAN example: null pointer access / null reference creation
#include <iostream>

struct Cat
{
    const char* speak() { return "meow"; }
};

int main()
{
    Cat* cat = nullptr;
    Cat& cat2 = *cat;
    std::cout << "Cat says: " << cat2.speak() << '\n';
    return 0;
}
