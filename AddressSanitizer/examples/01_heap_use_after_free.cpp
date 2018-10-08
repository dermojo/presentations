// ASAN example: heap use after free

#include <iostream>
#include <string>

struct Foo
{
    Foo(std::string x_) : x(x_) {}
    void print() { std::cout << "Foo has x=" << x << '\n'; }

    std::string x;
};

int main(int argc, char** argv)
{
    auto f = new Foo(argv[argc - 1]);
    if (argc % 2 == 0)
        delete f;
    f->print();

    return 0;
}
