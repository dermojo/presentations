#include <cstdio>
#include <cstring>

#ifdef __clang__
#pragma GCC diagnostic ignored "-Wreturn-stack-address"
#else
#pragma GCC diagnostic ignored "-Wreturn-local-addr"
#endif

// You need to run the test with ASAN_OPTIONS=detect_stack_use_after_return=1
// (works on Clang only)

const char* __attribute__((noinline)) toString(int i)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d", i);
    return buffer;
}

int main()
{
    return strlen(toString(5));
}
