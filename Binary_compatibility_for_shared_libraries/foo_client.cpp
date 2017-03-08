/**
 * Client program using the 'foo' library.
 */

#include "foo.hpp"

#include <iostream>

int main(int argc, const char** argv)
{
    int baz = -1;
    for (int i = 1; i < argc; ++i)
    {
        foo::FooClass myfoo(argv[i]);
        myfoo.doFoo();
        std::cout << myfoo.name();

        baz = atoi(argv[i]);
    }

    foo::FooClass myfoo(4711);
    myfoo.doFoo();
    std::cout << myfoo.name();
    std::cout << std::endl;

    foo::FooPod pod;
    foo::initPod(&pod);

    foo::VirtualFoo vfoo;
    try
    {
        vfoo.bar(baz);
    }
    catch (std::logic_error& exc)
    {
        std::cout << "VirtualFoo::bar() raised a logic_error: " << exc.what() << "\n";
    }
    catch (foo::FooError&)
    {
        std::cout << "VirtualFoo::bar() raised a FooError\n";
    }

    return 0;
}
