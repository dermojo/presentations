// UBSAN example: invalid object size

struct Base
{
    int x = 1;
};
struct Derived : Base
{
    int y = 2;
};

int main()
{
    Base b;
    Derived* d = static_cast<Derived*>(&b);
    return d->y;
}
