// ASAN example: use after scope

#include <string>

int main()
{
    std::string* s;
    {
        std::string text = "foo bar baz";
        s = &text;
    }
    *s = "Hello World";
    return 0;
}
