// UBSAN example: dynamic type violation
#include <iostream>

struct Animal
{
    virtual ~Animal() = default;
    virtual const char* speak() = 0;
};
struct Cat : public Animal
{
    const char* speak() override { return "meow"; }
};
struct Dog : public Animal
{
    const char* speak() override { return "woof"; }
};

int main()
{
    Dog dog;
    Cat* cat = reinterpret_cast<Cat*>(&dog);
    std::cout << "Cat says: " << cat->speak() << '\n';
    return 0;
}
