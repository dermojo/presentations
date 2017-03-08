/**
 * Public interface of the 'foo' library.
 *
 * Build the library and inspect its symbols with "objdump -T -C libfoo.so".
 */

#ifndef BINARY_COMPATIBILITY_FOR_SHARED_LIBRARIES_FOO_HPP_
#define BINARY_COMPATIBILITY_FOR_SHARED_LIBRARIES_FOO_HPP_

#include <stdexcept>

// changing this flag changes std::string fundamentally and breaks compatibility
//#define _GLIBCXX_USE_CXX11_ABI 0
#include <string>

/// namespace of the awesome 'foo' library
/// (should use '__attribute__((visibility("default")))')...
namespace foo
{

/// This is a FooClass. It does class things, but nothing virtual.
class FooClass
{
public:
    FooClass(const char* name);
    FooClass(unsigned int id);

    // inline: doesn't show up in the symbol table
    inline const std::string& name() const { return m_name; }

    bool doFoo();

private:
    /// the name of that foo
    std::string m_name;
};

// increase the size and see how an older program crashes...
#define WHAT_SIZE 32

/// simple POD
struct FooPod
{
    char what[WHAT_SIZE];
    int num;
};

/// static function with C linkage
extern "C" int initPod(FooPod* pod);

/// A Foo with virtual functions.
class VirtualFoo
{
public:
    virtual ~VirtualFoo();
    virtual size_t bar(int baz);
};

/// custom exception
class FooError : public std::exception
{
};
}

#endif /* BINARY_COMPATIBILITY_FOR_SHARED_LIBRARIES_FOO_HPP_ */
