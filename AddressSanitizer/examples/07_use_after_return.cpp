// ASAN example: use after return

// You need to run the test with "ASAN_OPTIONS=detect_stack_use_after_return=1",
// because this check is expensive both in CPU and RAM.
//
// Currently only implemented in Clang...

#include <cstdio>
#include <iostream>

#define NOINLINE __attribute__((noinline))

const char* NOINLINE makeGreeting(const char* name)
{
    char greeting[64];
    sprintf(greeting, "Hello, %s!", name);
    return greeting;
}

int main(int argc, char** argv)
{
    const char* g = makeGreeting(argv[1]);
    std::cout << g << '\n';
    return 0;
}
