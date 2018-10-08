#include <iostream>
#include <memory>

struct Foo
{
    Foo() { std::cout << "Foo constructed @" << this << "\n"; }
    ~Foo() { std::cout << "Foo destroyed @ " << this << "\n"; }
};

int main(int argc, char** argv)
{
    std::unique_ptr<Foo> x(new Foo());
    x.release();
    return 0;
}
