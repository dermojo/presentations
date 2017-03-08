/**
 * Implementation of the 'foo' library.
 */

#include "foo.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

namespace foo
{

// removing "static" exposes this function in the symbol table
static bool internalHelperFunc(const char* p)
{
    return p != nullptr;
}

FooClass::FooClass(const char* name) : m_name(name)
{
    internalHelperFunc(name);
}

FooClass::FooClass(unsigned int id) : m_name()
{
    // assume we don't have std::to_string()
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%u", id);
    m_name = buffer;
}

bool FooClass::doFoo()
{
    std::cout << "This is FooClass(" << m_name << ")\n";
    return m_name.find(':') == 47;
}

int initPod(FooPod* pod)
{
    if (pod)
    {
        memset(pod->what, 0, sizeof(pod->what));
        pod->num = -1;
        return 0;
    }
    return 1;
}

VirtualFoo::~VirtualFoo()
{
    std::cout << "No more VirtualFoo() ...\n";
}

size_t VirtualFoo::bar(int baz)
{
    if (baz < 0)
        return 4711;
    if (baz == 0)
        throw std::logic_error("baz cannot be 0");
    if (baz == 47)
        throw FooError();
    return static_cast<size_t>(baz);
}
}
